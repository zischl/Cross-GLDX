#include "CrossGLDX.h"
#include <glfw/glfw3.h>
#include <iostream>

int main() {
    std::cout << "Starting CrossGLDX Build Test..." << std::endl;
    CrossGLDX::PrintVersion();

#ifdef _WIN32
    std::cout << "Testing Win32Window..." << std::endl;
    WinConfig config(L"TestClass", 800, 600, L"Build Test Window", NULL);
    // Use the alias from CrossGLDX.h
    CrossGLDX::Window window;
    HWND syncHwnd = window.Create(config, GetModuleHandle(NULL), SW_HIDE);
    if (syncHwnd) {
        std::cout << "  [SUCCESS] Win32 window created." << std::endl;
        DestroyWindow(syncHwnd);
    }
#endif

    std::cout << "Build Test Completed Successfully." << std::endl;
    return 0;
}
