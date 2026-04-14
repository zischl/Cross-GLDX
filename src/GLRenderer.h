#ifndef GLRENDERER_H
#define GLRENDERER_H

#pragma once

#include "Logger.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <iostream>

enum GLTexPreset2D {
    GL_RenderTargetPreset,
    GL_DepthStencilPreset,
    GL_DynamicPreset,
    GL_StagingBufferPresetCPUR,
    GL_StagingBufferPresetCPUWR
};

struct GLBufferConfig {
    GLenum Usage = GL_STATIC_DRAW;
};

class GLRenderer {
public:
    GLRenderer();
    ~GLRenderer();

    bool Initialize();

    void SetViewport(int X, int Y, int Width, int Height);
    void Clear(float R, float G, float B, float A);

    GLuint CreateVertexBuffer(const void* Data, size_t Size, GLenum Usage = GL_STATIC_DRAW);
    GLuint CreateIndexBuffer(const void* Data, size_t Size, GLenum Usage = GL_STATIC_DRAW);

    GLuint CreateVertexShader(const char* Source);
    GLuint CreateFragmentShader(const char* Source);

    GLuint CreateVertexShaderFromFile(const std::string& FilePath);
    GLuint CreateFragmentShaderFromFile(const std::string& FilePath);

    GLuint CreateShaderProgram(GLuint VertexShader, GLuint FragmentShader);

    GLuint CreateVertexArray();
    void BindVertexArray(GLuint Vao);

    GLuint CreateTexture2D(int Width, int Height, GLTexPreset2D Preset);
    GLuint CreateFramebuffer(GLuint ColorAttachment, GLuint DepthAttachment = 0);

private:
    std::string ReadFile(const std::string& FilePath);
    void CheckShaderCompileStatus(GLuint Shader, const std::string& Type);
    void CheckProgramLinkStatus(GLuint Program);
};

#endif
