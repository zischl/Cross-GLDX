#include "Win32Window.h"
#include "Logger.h"

// --- Win32Window (Sync) Implementation ---

Win32Window::Win32Window(WNDPROC WindowProc) : WProc(WindowProc) {}

HWND Win32Window::Create(WinConfig& Config, HINSTANCE hInstance, int nCmdShow) {
	m_hInstance = hInstance;
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = WProc;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = Config.class_name;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (!GetClassInfoExW(m_hInstance, Config.class_name, &wc)) {
		if (RegisterClassExW(&wc) == 0) {
			Logger::log(L"Window Class Registration Failed");
			return NULL;
		}
	}

	hwnd = CreateWindowExW(
		0,
		Config.class_name,
		Config.Window_Name,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		Config.wdWidth,
		Config.wdHeight,
		nullptr,
		NULL,
		m_hInstance,
		Config.lParam
	);

	SetProcessDPIAware();

	if (hwnd == NULL) {
		Logger::log(L"Window Creation Failed");
		Logger::log(std::to_wstring(GetLastError()));
		return NULL;
	}

	return hwnd;
}

LRESULT CALLBACK Win32Window::WProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return TRUE;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


Win32AsyncWindow::Win32AsyncWindow(WNDPROC WindowProc) : Win32Window(WindowProc) {}

Win32AsyncWindow::~Win32AsyncWindow() {
	if (m_Events) {
		delete[] m_Events;
	}
}

HWND Win32AsyncWindow::CreateAsync(const wchar_t* window_name, HINSTANCE& hInstance, int nCmdShow, D3DDevice D3DDevStruct) {
	std::thread renderThread([&, window_name, nCmdShow] {
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		
		WinConfig config(L"Win32WindowClass", 1920, 1080, window_name, NULL);
		hwnd = Create(config, hInstance, nCmdShow);
		
		if (!hwnd) return;

		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);

		m_Events = new HANDLE[1];
		m_Events[0] = CreateEvent(NULL, FALSE, TRUE, "WF_RENDER");

		// D3D11 Initialization
		m_D3D11Device = D3DDevStruct.D3D11Device.Get();
		m_D3D11Context = D3DDevStruct.D3D11Context.Get();
		
		if (m_D3D11Context) {
			m_ContextMode = m_D3D11Context->GetType();
		}

		MainLoop();
		
		CoUninitialize();
	});

	renderThread.detach();
	return hwnd;
}

void Win32AsyncWindow::Render() {
	if (!m_D3D11Device || !m_D3D11Context) return;

	m_D3D11Context->ClearRenderTargetView(m_RenderTargetView, m_ClearColor);
	
	if (m_Swapchain) {
		m_Swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
	}
}

void Win32AsyncWindow::MainLoop() {
	while (true) {
		m_EventDW = MsgWaitForMultipleObjectsEx(1, m_Events, 1, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

		switch (m_EventDW) {
		case WAIT_OBJECT_0 + 1: 
			while (PeekMessage(&m_Msg, nullptr, 0, 0, PM_REMOVE)) {
				if (m_Msg.message == WM_QUIT)
					return;

				TranslateMessage(&m_Msg);
				DispatchMessage(&m_Msg);
			}
			break;

		case WAIT_OBJECT_0 + 0: 
			Render();
			break;

		case WAIT_TIMEOUT:
			break;
		}
	}
}
