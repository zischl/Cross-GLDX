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

struct GLRasterizerState {
    GLenum FillMode = GL_FILL;
    GLenum CullMode = GL_BACK;
    bool ScissorEnable = false;
};

struct GLBlendState {
    bool BlendEnable = false;
    GLenum SrcBlend = GL_ONE;
    GLenum DestBlend = GL_ZERO;
    GLenum BlendOp = GL_FUNC_ADD;
};

struct GLDepthStencilState {
    bool DepthEnable = true;
    bool DepthWriteMask = GL_TRUE;
    GLenum DepthFunc = GL_LESS;
};

class GLRenderer {
public:
    GLRenderer();
    ~GLRenderer();

    bool Initialize();

    void Clear(float r, float g, float b, float a);
    void SetViewport(int x, int y, int width, int height);
    void Present();

    GLuint CreateVertexBuffer(const void* Data, size_t Size, GLenum Usage = GL_STATIC_DRAW);
    GLuint CreateIndexBuffer(const void* Data, size_t Size, GLenum Usage = GL_STATIC_DRAW);
    GLuint CreateUniformBuffer(const void* Data, size_t Size, GLenum Usage = GL_DYNAMIC_DRAW);

    GLuint CreateVertexShader(const char* Source);
    GLuint CreateFragmentShader(const char* Source);

    GLuint CreateVertexShaderFromFile(const std::string& FilePath);
    GLuint CreateFragmentShaderFromFile(const std::string& FilePath);

    GLuint CreateShaderProgram(GLuint VertexShader, GLuint FragmentShader);

    GLuint CreateVertexArray();
    void BindVertexArray(GLuint Vao);

    GLuint CreateTexture2D(int Width, int Height, GLTexPreset2D Preset);
    GLuint CreateFramebuffer(GLuint ColorAttachment, GLuint DepthAttachment = 0);

    void SetRasterizerState(const GLRasterizerState& State);
    void SetBlendState(const GLBlendState& State);
    void SetDepthStencilState(const GLDepthStencilState& State);

private:
    std::string ReadFile(const std::string& FilePath);
    void CheckShaderCompileStatus(GLuint Shader, const std::string& Type);
    void CheckProgramLinkStatus(GLuint Program);
};

#endif
