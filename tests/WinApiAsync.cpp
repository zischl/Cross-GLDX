#include "D3D11Renderer.h"
#include "Win32Window.h"

#include <iostream>
#include <chrono>
#include <thread>



int main() {
    std::cout << "Starting Asynchronous Window Example..." << std::endl;

    HINSTANCE hInstance = GetModuleHandle(NULL);
    
    // Initializing the Renderer to get the custtom device struct
    D3D11Renderer Renderer;
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3DDevice devStruct;
    if (FAILED(Renderer.CreateD3d11Device(featureLevels, _countof(featureLevels), creationFlags, &devStruct))) {
        std::cerr << "Failed to create D3D11 Device!" << std::endl;
        return -1;
    }

    // Creating Win32AsyncWindow instance and launching async window
    Win32AsyncWindow forge;
    
    // The window will be created in its own thread and handle its own message loop
    HWND asyncHwnd = forge.CreateAsync(L"AsyncNexusWindow", hInstance, SW_SHOW, devStruct);

    if (asyncHwnd) {
        std::cout << "Async window initiated. Main thread is free." << std::endl;
    }

    // Keeping dummy main thread alive for a bit to show the window
    
    while (true) {
        std::string input;
        std::cout << "Enter q to quit : ";
        std::cin >> input;

        if (input == "q") {
            break;
        }
        
    }

    std::cout << "Exiting Async Example." << std::endl;
    return 0;
}
