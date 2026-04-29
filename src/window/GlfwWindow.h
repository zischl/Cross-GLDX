#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H

#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

struct WindowConfig {
    std::wstring title = L"CrossGLDX Window";
    int width = 1280;
    int height = 720;
    bool visible = true;
};

class GlfwWindow {
public:
    GlfwWindow();
    ~GlfwWindow();

    bool Create(const WindowConfig& config);
    void Update();
    bool ShouldClose() const;
    void Close();

    GLFWwindow* GetNativeWindow() const { return m_Window; }

private:
    GLFWwindow* m_Window = nullptr;
};

#endif
