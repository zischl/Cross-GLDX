#include "WinForge.h"

WinForge::WinForge(WNDPROC WindowProc) {
	WProc = WindowProc;
}


HWND WindowInit(WinConfig& Config, HINSTANCE hInstance, int nCmdShow, WNDPROC WProc) {
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = WProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = Config.class_name;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (!GetClassInfoExW(hInstance, Config.class_name, &wc)) {
		if (RegisterClassExW(&wc) == 0) {
			OutputDebugString(L"Window Class Registration Failed\n");
			return NULL;
		}
	}

	HWND hwnd_ = CreateWindowExW(
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
		hInstance,
		Config.lParam
	);

	SetProcessDPIAware();

	if (hwnd_ == NULL)
	{
		OutputDebugString(L"Window Creation Failed\n");
		OutputDebugString((std::to_wstring(GetLastError()) + L"\n").c_str());
		return NULL;
	}

	return hwnd_;
}

HWND WinForge::CreateWindowAsync(const wchar_t* window_name, HINSTANCE& hInstance, int nCmdShow, D3DDevice D3DDevStruct) {

	std::thread renderThread([&, window_name, nCmdShow] {
		hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		
		WinConfig config(L"WinForgeClass", 1920, 1080, window_name, NULL);
		hwnd = WindowInit(config, hInstance, nCmdShow, WProc);
		
		if (!hwnd) return;

		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);

		Events = new HANDLE[1];
		Events[0] = CreateEvent(NULL, FALSE, TRUE, L"WF_RENDER");

		// D3D11 Initialization (Minimal)
		D3D11Device = D3DDevStruct.D3D11Device.Get();
		D3D11Context = D3DDevStruct.D3D11Context.Get();
		
		if (D3D11Context) {
			ContextMode = D3D11Context->GetType();
		}

		MainLoop();
		
		CoUninitialize();
	});

	renderThread.detach();

	return hwnd;
}


void WinForge::Render() {
	if (!D3D11Device || !D3D11Context) return;

	// Basic Render Logic
	D3D11Context->ClearRenderTargetView(renderTargetView, clearColor);
	
	if (swapchain) {
		swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
	}
}

void WinForge::MainLoop() {
	while (true) {
		EventDW = MsgWaitForMultipleObjectsEx(1, Events, 1, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

		switch (EventDW) {
		case WAIT_OBJECT_0 + 1: // Message in queue
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT)
					return;

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			break;

		case WAIT_OBJECT_0 + 0: // Render event
			Render();
			break;

		case WAIT_TIMEOUT:
			break;
		}
	}
}


LRESULT CALLBACK WinForge::WProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
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
