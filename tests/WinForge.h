#ifndef UNICODE
#define UNICODE
#endif 

#ifndef WINFORGE_H
#define WINFORGE_H


#pragma once


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <comdef.h>
#include <wrl/client.h>

#include <string>
#include <future>
#include <mutex>
#include <thread>

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


HWND WindowInit(WinConfig& Config, HINSTANCE hInstance, int nCmdShow, WNDPROC WProc);

class WinForge {
public:
	static LRESULT CALLBACK WProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	WinForge(WNDPROC WindowProc = WProc2);

	
	HWND CreateWindowAsync(const wchar_t* window_name, HINSTANCE& hInstance, int nCmdShow, D3DDevice D3DDevStruct = {});
	

	inline void SetRenderEvent() {
		SetEvent(Events[0]);
	}
	//void ContextSwitch();

	inline void SetFPSLimit(int FPS) {
		limit.store(1000 / FPS);
	}


private:
	HRESULT hr = NULL;
	HWND hwnd = NULL;
	WNDPROC WProc = NULL;
	HANDLE* Events = nullptr;
	DWORD EventDW = NULL;

	std::chrono::steady_clock::duration FrameTimeLimit = std::chrono::nanoseconds(15 * 1000000);

	std::chrono::time_point<std::chrono::steady_clock> LastFrameTime = std::chrono::steady_clock::now();


	ID3D11Device* D3D11Device = nullptr;
	ID3D11DeviceContext* D3D11Context = nullptr;

	IDXGISwapChain3* swapchain = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;

	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
	ID3D11SamplerState* sampler = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	ComPtr<ID3D11ShaderResourceView> textureView = nullptr;

	UINT stride = 0;
	UINT offset = 0;

	float clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };




	D3D11_TEXTURE2D_DESC custommainBufferDesc = {};


	D3D11_DEVICE_CONTEXT_TYPE ContextMode = D3D11_DEVICE_CONTEXT_IMMEDIATE;

	std::atomic<int> limit = 7;
	MSG msg = { };

	
	void Render();
	void MainLoop();

	__forceinline void null() {}
};

#endif