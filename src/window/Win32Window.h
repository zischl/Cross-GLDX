#ifndef WIN32WINDOW_H
#define WIN32WINDOW_H
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <comdef.h>
#include <wrl/client.h>
#include <string>
#include <future>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>

#include "D3D11Renderer.h"

using Microsoft::WRL::ComPtr;

struct WinConfig {
	const wchar_t* class_name = L"Something";
	const wchar_t* Window_Name = L"Window";
	UINT wdWidth = 1280;
	UINT wdHeight = 720;
	LPVOID lParam = NULL;

	WinConfig(const wchar_t* ClassName, UINT Width, UINT Height, const wchar_t* WindowName, LPVOID lParam_) :
		class_name(ClassName),
		Window_Name(WindowName),
		wdWidth(Width),
		wdHeight(Height),
		lParam(lParam_) {}
};


class Win32Window {
public:
	static LRESULT CALLBACK WProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	Win32Window(WNDPROC WindowProc = WProc2);
	virtual ~Win32Window() = default;

	HWND Create(WinConfig& Config, HINSTANCE hInstance, int nCmdShow);
	HWND GetNativeWindow() const { return hwnd; }

protected:
	HWND hwnd = NULL;
	HINSTANCE m_hInstance = NULL;
	WNDPROC WProc = NULL;
};

class Win32AsyncWindow : public Win32Window {
public:
	Win32AsyncWindow(WNDPROC WindowProc = WProc2);
	~Win32AsyncWindow();

	HWND CreateAsync(const wchar_t* window_name, HINSTANCE& hInstance, int nCmdShow, D3DDevice D3DDevStruct = {});

	void SetRenderEvent() {
		if (m_Events) SetEvent(m_Events[0]);
	}

	void SetFPSLimit(int FPS) {
		m_LimitPercentage.store(1000 / FPS);
	}

private:
	HANDLE* m_Events = nullptr;
	DWORD m_EventDW = 0;
	std::atomic<int> m_LimitPercentage{7};
	MSG m_Msg = {};

	ID3D11Device* m_D3D11Device = nullptr;
	ID3D11DeviceContext* m_D3D11Context = nullptr;
	IDXGISwapChain3* m_Swapchain = nullptr;
	ID3D11RenderTargetView* m_RenderTargetView = nullptr;
	
	D3D11_DEVICE_CONTEXT_TYPE m_ContextMode = D3D11_DEVICE_CONTEXT_IMMEDIATE;
	float m_ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

	void Render();
	void MainLoop();
};

#endif