#ifndef GLDX_RENDERER_CONTEXT_H
#define GLDX_RENDERER_CONTEXT_H

#pragma once

#include "RendererHandle.h"
#include "ResourcePool.h"
#include <stdint.h>

#ifdef _WIN32
#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_5.h>
using Microsoft::WRL::ComPtr;
#endif

#include <glad/glad.h>

#ifndef GLDX_MAX_BUFFERS
#define GLDX_MAX_BUFFERS 1024
#endif

#ifndef GLDX_MAX_TEXTURES
#define GLDX_MAX_TEXTURES 512
#endif

#ifndef GLDX_MAX_SHADERS
#define GLDX_MAX_SHADERS 256
#endif

#ifndef GLDX_MAX_PIPELINES
#define GLDX_MAX_PIPELINES 128
#endif

#ifndef GLDX_MAX_SAMPLERS
#define GLDX_MAX_SAMPLERS 64
#endif

namespace CrossGLDX {

    enum class GraphicsAPIType : uint8_t {
        D3D11,
        OpenGL
    };

    struct PipelineSlot {
#ifdef _WIN32
        ID3D11InputLayout D3DInputLayout;
        ID3D11VertexShader D3DVS;
        ID3D11PixelShader D3DPS;
        ID3D11BlendState D3DBlend;
        ID3D11DepthStencilState D3DDepthStencil;
        ID3D11RasterizerState D3DRasterizer;
#endif
        GLuint GLVAO = 0;
        GLuint GLProgram = 0;
    };

    struct RendererContext {
        GraphicsAPIType GraphicsAPI;

#ifdef _WIN32
        ID3D11Device D3DDevice;
        ID3D11DeviceContext D3DContext;
        IDXGISwapChain3 D3DSwapChain;
        ID3D11RenderTargetView D3DMainRTV;
        ID3D11DepthStencilView D3DMainDSV;

        ResourcePool<ID3D11Buffer, GLDX_MAX_BUFFERS>  D3DBuffers;
        ResourcePool<ID3D11Texture2D, GLDX_MAX_TEXTURES> D3DTextures;
        ResourcePool<ID3DBlob, GLDX_MAX_SHADERS>  D3DShaders;
        ResourcePool<PipelineSlot, GLDX_MAX_PIPELINES> D3DPipelines;
        ResourcePool<ID3D11SamplerState, GLDX_MAX_SAMPLERS> D3DSamplers;
#endif

        ResourcePool<GLuint, GLDX_MAX_BUFFERS>  GLBuffers;
        ResourcePool<GLuint, GLDX_MAX_TEXTURES> GLTextures;
        ResourcePool<GLuint, GLDX_MAX_SHADERS>  GLShaders;
        ResourcePool<PipelineSlot, GLDX_MAX_PIPELINES> GLPipelines;
        ResourcePool<GLuint, GLDX_MAX_SAMPLERS> GLSamplers;

        RendererContext(GraphicsAPIType type) : GraphicsAPI(type) {}
    };

}

#endif
