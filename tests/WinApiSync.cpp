#include "Win32Window.h"
#include "D3D11Renderer.h"
#include <iostream>
#include <chrono>

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    int nCmdShow = SW_SHOW;

    // Seting up Events
    HANDLE Events[1];
    Events[0] = CreateEvent(NULL, FALSE, TRUE, "PanelRender");

    // Window Configuration
    WinConfig config(L"Example Window", 1280, 720, L"WinApi Window Example", NULL);

    // Initializing The Window
    Win32Window Window;
    HWND hwnd = Window.Create(config, hInstance, nCmdShow);
    if (!hwnd) {
        std::cerr << "Failed to create window!" << std::endl;
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Initializing Renderer
    D3D11Renderer Renderer;
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    D3DDevice devStruct;
    if (FAILED(Renderer.CreateD3d11Device(featureLevels, _countof(featureLevels), creationFlags, &devStruct))) {
        std::cerr << "Failed to create D3D11 Device!" << std::endl;
        return -1;
    }
    ID3D11Device* D3D11Device = devStruct.D3D11Device.Get();
    ID3D11DeviceContext* D3D11Context = devStruct.D3D11Context.Get();

    // Setup for SwapChain and RTV
    ComPtr<IDXGIFactory2> factory;
    Renderer.CreateDXGIFactory2(&factory);

    ComPtr<IDXGISwapChain3> swapchain;
    Renderer.CreateSwapChain(D3D11Device, hwnd, 1280, 810, factory.Get(), {}, &swapchain);

    ComPtr<ID3D11Texture2D> backBuffer;
    Renderer.GetSwapChainBuffer(swapchain.Get(), 0, &backBuffer);

    ComPtr<ID3D11RenderTargetView> renderTargetView;
    Renderer.CreateRTV(D3D11Device, backBuffer.Get(), &renderTargetView);

    // Main Loop
    MSG msg = {};
    auto LastFrameTime = std::chrono::steady_clock::now();
    // Roughly setting the fps limit with timer to smth around 60 fps
    auto FrameTimeLimit = std::chrono::milliseconds(16);
    float clearColor[4] = { 0.1f, 0.2f, 0.4f, 1.0f };

    std::cout << "Starting Synchronous Main Loop..." << std::endl;

    while (true) {
        DWORD EventDW = MsgWaitForMultipleObjectsEx(1, Events, 10, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

        switch (EventDW) {
        case WAIT_OBJECT_0 + 1:
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT)
                    goto end_loop;

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            break;

        case WAIT_OBJECT_0 + 0:
        case WAIT_TIMEOUT:
            if (std::chrono::steady_clock::now() - LastFrameTime >= FrameTimeLimit) {
                D3D11Context->ClearRenderTargetView(renderTargetView.Get(), clearColor);
                D3D11Context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

                swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING);

                LastFrameTime = std::chrono::steady_clock::now();
            }
            break;
        }
    }

end_loop:
    std::cout << "Exiting Sync Example." << std::endl;
    return 0;
}
