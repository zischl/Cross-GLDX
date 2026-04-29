#ifndef CROSSGLDX_H
#define CROSSGLDX_H

#pragma once

#include "Logger.h"
#include "RendererTypes.h"
#include "GLRenderer.h"
#include "GlfwWindow.h"

#ifdef _WIN32
#include "Win32Window.h"
#include "D3D11Renderer.h"
#endif

namespace CrossGLDX {

#ifdef _WIN32
    // Default to high-performance Windows 
    using Window = Win32Window;
    using AsyncWindow = Win32AsyncWindow;
    using Renderer = D3D11Renderer;
    using Buffer = D3D11Buffer;

#else
    // Default to Cross-Platform paths
    using Window = GlfwWindow;
    using Renderer = GLRenderer;
    using Buffer = GLBuffer;
#endif

    inline void PrintVersion() {
        Logger::log("CrossGLDX Version 0.1.0");
    }
}

#endif