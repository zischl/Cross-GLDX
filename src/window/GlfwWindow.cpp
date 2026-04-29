#include "GlfwWindow.h"
#include "Logger.h"
#include <iostream>

#include <GLFW/glfw3.h>

GlfwWindow::GlfwWindow() {}

GlfwWindow::~GlfwWindow() {
    Close();
}

bool GlfwWindow::Create(const WindowConfig& config) {
    if (!glfwInit()) {
        Logger::log("Failed to initialize GLFW");
        return false;
    }


    // Converting wstring to string for GLFW
    std::string titleStr(config.title.begin(), config.title.end());

    glfwWindowHint(GLFW_VISIBLE, config.visible ? GLFW_TRUE : GLFW_FALSE);
    
    // User can set hints before Create
    m_Window = glfwCreateWindow(config.width, config.height, titleStr.c_str(), nullptr, nullptr);
    
    if (!m_Window) {
        Logger::log("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    return true;
}

void GlfwWindow::Update() {
    if (m_Window) {
        glfwPollEvents();
    }
}

bool GlfwWindow::ShouldClose() const {
    return m_Window ? glfwWindowShouldClose(m_Window) : true;
}

void GlfwWindow::Close() {
    if (m_Window) {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
}
