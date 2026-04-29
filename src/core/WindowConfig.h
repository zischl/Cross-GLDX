#ifndef CG_WINDOW_CONFIG_H
#define CG_WINDOW_CONFIG_H

#pragma once
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace CrossGLDX {

    struct WindowConfig {
        std::wstring title = L"CrossGLDX Window";
        int width = 1280;
        int height = 720;
        bool visible = true;
        
#ifdef _WIN32
        const wchar_t* className = L"CrossGLDX_Class";
        HINSTANCE hInstance = NULL;
        int nCmdShow = 1;
        void* lParam = nullptr;
#endif
    };

}

#endif
