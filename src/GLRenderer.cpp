#include "GLRenderer.h"
#include <fstream>
#include <sstream>

GLRenderer::GLRenderer() {}

GLRenderer::~GLRenderer() {}

bool GLRenderer::Initialize() {
    if (!gladLoadGL()) {
        Logger::log("Failed to initialize GLAD");
        return false;
    }

    Logger::log("GLAD initialized. OpenGL Version: " + std::string((const char*)glGetString(GL_VERSION)));
    return true;
}

void GLRenderer::SetViewport(int X, int Y, int Width, int Height) {
    glViewport(X, Y, Width, Height);
}

void GLRenderer::Clear(float R, float G, float B, float A) {
    glClearColor(R, G, B, A);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

GLuint GLRenderer::CreateVertexBuffer(const void* Data, size_t Size, GLenum Usage) {
    GLuint Buffer;
    glCreateBuffers(1, &Buffer);
    glNamedBufferStorage(Buffer, Size, Data, GL_DYNAMIC_STORAGE_BIT);
    return Buffer;
}

GLuint GLRenderer::CreateIndexBuffer(const void* Data, size_t Size, GLenum Usage) {
    GLuint Buffer;
    glCreateBuffers(1, &Buffer);
    glNamedBufferStorage(Buffer, Size, Data, GL_DYNAMIC_STORAGE_BIT);
    return Buffer;
}

GLuint GLRenderer::CreateVertexShader(const char* Source) {
    GLuint Shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Shader, 1, &Source, nullptr);
    glCompileShader(Shader);
    CheckShaderCompileStatus(Shader, "VERTEX");
    return Shader;
}

GLuint GLRenderer::CreateFragmentShader(const char* Source) {
    GLuint Shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(Shader, 1, &Source, nullptr);
    glCompileShader(Shader);
    CheckShaderCompileStatus(Shader, "FRAGMENT");
    return Shader;
}

GLuint GLRenderer::CreateVertexShaderFromFile(const std::string& FilePath) {
    std::string Source = ReadFile(FilePath);
    return CreateVertexShader(Source.c_str());
}

GLuint GLRenderer::CreateFragmentShaderFromFile(const std::string& FilePath) {
    std::string Source = ReadFile(FilePath);
    return CreateFragmentShader(Source.c_str());
}

GLuint GLRenderer::CreateShaderProgram(GLuint VertexShader, GLuint FragmentShader) {
    GLuint Program = glCreateProgram();
    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    glLinkProgram(Program);
    CheckProgramLinkStatus(Program);
    return Program;
}

GLuint GLRenderer::CreateVertexArray() {
    GLuint Vao;
    glCreateVertexArrays(1, &Vao);
    return Vao;
}

void GLRenderer::BindVertexArray(GLuint Vao) {
    glBindVertexArray(Vao);
}

GLuint GLRenderer::CreateTexture2D(int Width, int Height, GLTexPreset2D Preset) {
    GLuint Texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &Texture);

    GLenum InternalFormat = GL_RGBA8;
    GLenum Format = GL_RGBA;
    GLenum Type = GL_UNSIGNED_BYTE;

    switch (Preset) {
    case GL_RenderTargetPreset:
        InternalFormat = GL_RGBA8;
        break;
    case GL_DepthStencilPreset:
        InternalFormat = GL_DEPTH24_STENCIL8;
        break;
    case GL_DynamicPreset:
        InternalFormat = GL_RGBA8;
        break;
    case GL_StagingBufferPresetCPUR:
        InternalFormat = GL_RGBA8;
        break;
    case GL_StagingBufferPresetCPUWR:
        InternalFormat = GL_RGBA8;
        break;
    }

    glTextureStorage2D(Texture, 1, InternalFormat, Width, Height);
    glTextureParameteri(Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return Texture;
}

GLuint GLRenderer::CreateFramebuffer(GLuint ColorAttachment, GLuint DepthAttachment) {
    GLuint Fbo;
    glCreateFramebuffers(1, &Fbo);
    if (ColorAttachment) {
        glNamedFramebufferTexture(Fbo, GL_COLOR_ATTACHMENT0, ColorAttachment, 0);
    }
    if (DepthAttachment) {
        glNamedFramebufferTexture(Fbo, GL_DEPTH_STENCIL_ATTACHMENT, DepthAttachment, 0);
    }

    if (glCheckNamedFramebufferStatus(Fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::log("Framebuffer is not complete!");
    }

    return Fbo;
}

std::string GLRenderer::ReadFile(const std::string& FilePath) {
    std::ifstream File(FilePath);
    if (!File.is_open()) {
        Logger::log("Failed to open file: " + FilePath);
        return "";
    }
    std::stringstream Buffer;
    Buffer << File.rdbuf();
    return Buffer.str();
}

void GLRenderer::CheckShaderCompileStatus(GLuint Shader, const std::string& Type) {
    GLint Success;
    GLchar InfoLog[1024];
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
    if (!Success) {
        glGetShaderInfoLog(Shader, 1024, nullptr, InfoLog);
        Logger::log("ERROR::SHADER_COMPILATION_ERROR of type: " + Type + "\n" + InfoLog);
    }
}

void GLRenderer::CheckProgramLinkStatus(GLuint Program) {
    GLint Success;
    GLchar InfoLog[1024];
    glGetProgramiv(Program, GL_LINK_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(Program, 1024, nullptr, InfoLog);
        Logger::log("ERROR::PROGRAM_LINKING_ERROR\n" + std::string(InfoLog));
    }
}