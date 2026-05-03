#ifndef GLDX_RENDERER_H
#define GLDX_RENDERER_H

#include "RendererContext.h"
#pragma once

#include "RendererDescriptors.h"
#include "RendererHandle.h"
#include <stddef.h>

namespace CrossGLDX {

struct RendererConfig {
  GraphicsAPIType api = GraphicsAPIType::OpenGL;
  uint32_t width = 1280;
  uint32_t height = 720;
  void *windowHandle = nullptr;
};

struct RendererContext;

RendererContext *CreateContext(const RendererConfig &config);
void DestroyContext(RendererContext *ctx);

BufferHandle CreateBuffer(RendererContext *ctx, const BufferDesc &desc,
                          const void *data);

void DestroyBuffer(RendererContext *ctx, BufferHandle handle);

void UpdateBuffer(RendererContext *ctx, BufferHandle buffer, const void *data,
                  size_t size);

void BeginFrame(RendererContext *ctx);
void EndFrame(RendererContext *ctx);
void Clear(RendererContext *ctx, float r, float g, float b, float a);

void SetVertexBuffer(RendererContext *ctx, uint32_t slot, BufferHandle buffer,
                     uint32_t stride, uint32_t offset);
void SetIndexBuffer(RendererContext *ctx, BufferHandle buffer, uint32_t offset);

void Draw(RendererContext *ctx, uint32_t vertexCount, uint32_t startVertex);
void DrawIndexed(RendererContext *ctx, uint32_t indexCount, uint32_t startIndex,
                 int32_t baseVertex);

#ifdef _WIN32
struct ID3D11Buffer *GetNativeD3D11Buffer(RendererContext *ctx,
                                          BufferHandle handle);
struct ID3D11Device *GetNativeD3D11Device(RendererContext *ctx);
#endif
unsigned int GetNativeGLBuffer(RendererContext *ctx, BufferHandle handle);

} // namespace CrossGLDX

#endif
