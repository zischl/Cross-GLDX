#include "Renderer.h"
#include "Logger.h"
#include "RendererContext.h"
#include "RendererHandle.h"
#include <cstdint>

#ifdef _WIN32
#include <d3d11.h>
#include <dxgi1_5.h>
#endif

#include <assert.h>
#include <glad/glad.h>

namespace CrossGLDX {

RendererContext *CreateContext(const RendererConfig &config) {
  RendererContext *ctx = new RendererContext(config.api);

  if (config.api == GraphicsAPIType::D3D11) {
#ifdef _WIN32
    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0};
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr =
        D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                          creationFlags, featureLevels, 1, D3D11_SDK_VERSION,
                          &ctx->D3DDevice, nullptr, &ctx->D3DContext);

    if (FAILED(hr)) {
      Logger::log("Failed to create D3D11 Device");
      delete ctx;
      return nullptr;
    }

#endif
  } else {
    if (!gladLoadGL()) {
      Logger::log("Failed to initialize GLAD");
      delete ctx;
      return nullptr;
    }
  }

  return ctx;
}

void DestroyContext(RendererContext *ctx) {
  if (!ctx)
    return;

  delete ctx;
}

BufferHandle CreateBuffer(RendererContext *ctx, const BufferDesc &desc,
                          const void *data) {
  if (ctx->GraphicsAPI == GraphicsAPIType::D3D11) {
#ifdef _WIN32
    auto handle = ctx->D3DBuffers.Alloc();
    uint32_t &idx = handle.idx;
    uint32_t &generation = handle.generation;

    if (idx == INVALID_BUFFER)
      return INVALID_BUFFER;

    D3D11_BUFFER_DESC d3dDesc = {};
    d3dDesc.ByteWidth = (UINT)desc.size;
    d3dDesc.MiscFlags = 0;
    d3dDesc.StructureByteStride = 0;

    switch (desc.type) {
    case BufferType::Vertex:
      d3dDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      break;
    case BufferType::Index:
      d3dDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      break;
    case BufferType::Constant:
      d3dDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      break;
    case BufferType::Structured:
      d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
      d3dDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
      break;
    }

    switch (desc.access) {
    case BufferAccess::GPUOnly:
      d3dDesc.Usage = D3D11_USAGE_DEFAULT;
      d3dDesc.CPUAccessFlags = 0;
      break;
    case BufferAccess::CPUWrite:
      d3dDesc.Usage = D3D11_USAGE_DYNAMIC;
      d3dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      break;
    case BufferAccess::CPUReadWrite:
      d3dDesc.Usage = D3D11_USAGE_STAGING;
      d3dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
      break;
    }

    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem = data;

    HRESULT hr =
        ctx->D3DDevice->CreateBuffer(&d3dDesc, data ? &subData : nullptr,
                                     &ctx->D3DBuffers.slots[idx].resource);
    if (FAILED(hr)) {
      ctx->D3DBuffers.Free(idx);
      return INVALID_BUFFER;
    }

    return BufferHandle(idx, generation);
#endif
  } else {
    auto handle = ctx->GLBuffers.Alloc();
    uint32_t idx = handle.idx;
    uint32_t generation = handle.idx;

    if (idx == INVALID_BUFFER)
      return INVALID_BUFFER;

    GLuint buffer;
    glCreateBuffers(1, &buffer);

    GLenum flags = 0;
    if (desc.access == BufferAccess::CPUWrite) {
      flags |= GL_DYNAMIC_STORAGE_BIT;
    }

    glNamedBufferStorage(buffer, desc.size, data, flags);
    ctx->GLBuffers.slots[idx].resource = buffer;

    return BufferHandle(idx, generation);
  }
  return INVALID_BUFFER;
}

void UpdateBuffer(RendererContext *ctx, BufferHandle buffer, const void *data,
                  size_t size) {
  if (buffer == INVALID_BUFFER)
    return;

  if (ctx->GraphicsAPI == GraphicsAPIType::D3D11) {
#ifdef _WIN32
    ID3D11Buffer *d3dBuffer = ctx->D3DBuffers.slots[buffer.idx].resource;
    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr =
        ctx->D3DContext->Map(d3dBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (SUCCEEDED(hr)) {
      memcpy(mapped.pData, data, size);
      ctx->D3DContext->Unmap(d3dBuffer, 0);
    }
#endif
  } else {
    GLuint glBuffer = ctx->GLBuffers.slots[buffer.idx].resource;
    glNamedBufferSubData(glBuffer, 0, size, data);
  }
}

void BeginFrame(RendererContext *ctx) {
  if (ctx->GraphicsAPI == GraphicsAPIType::D3D11) {
#ifdef _WIN32
    if (ctx->D3DMainRTV) {
      ctx->D3DContext->OMSetRenderTargets(1, ctx->D3DMainRTV.GetAddressOf(),
                                          ctx->D3DMainDSV.Get());
    }
#endif
  } else {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
}

void EndFrame(RendererContext *ctx) {
  // swap buffers maybe ? not sure
}

void Clear(RendererContext *ctx, float r, float g, float b, float a) {
  if (ctx->GraphicsAPI == GraphicsAPIType::D3D11) {
#ifdef _WIN32
    if (ctx->D3DMainRTV) {
      float color[4] = {r, g, b, a};
      ctx->D3DContext->ClearRenderTargetView(ctx->D3DMainRTV.Get(), color);
    }
    if (ctx->D3DMainDSV) {
      ctx->D3DContext->ClearDepthStencilView(
          ctx->D3DMainDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f,
          0);
    }
#endif
  } else {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }
}

void DestroyBuffer(RendererContext *ctx, BufferHandle handle) {
  if (handle == INVALID_BUFFER)
    return;
  if (ctx->GraphicsAPI == GraphicsAPIType::D3D11) {
#ifdef _WIN32
    ctx->D3DBuffers.slots[handle.idx];
    ctx->D3DBuffers.Free(handle.idx);
#endif
  } else {
    glDeleteBuffers(1, &ctx->GLBuffers.slots[handle.idx].resource);
    ctx->GLBuffers.Free(handle.idx);
  }
}

void SetVertexBuffer(RendererContext *ctx, uint32_t slot, BufferHandle buffer,
                     uint32_t stride, uint32_t offset) {
  if (buffer == INVALID_BUFFER)
    return;
  if (ctx->GraphicsAPI == GraphicsAPIType::D3D11) {
#ifdef _WIN32
    UINT uStride = stride;
    UINT uOffset = offset;
    ctx->D3DContext->IASetVertexBuffers(
        slot, 1, &ctx->D3DBuffers.slots[buffer.idx].resource, &uStride,
        &uOffset);
#endif
  } else {
    glBindVertexBuffer(slot, ctx->GLBuffers.slots[buffer.idx].resource, offset,
                       stride);
  }
}

void SetIndexBuffer(RendererContext *ctx, BufferHandle buffer,
                    uint32_t offset) {
  if (buffer == INVALID_BUFFER)
    return;
  if (ctx->GraphicsAPI == GraphicsAPIType::D3D11) {
#ifdef _WIN32
    ctx->D3DContext->IASetIndexBuffer(
        ctx->D3DBuffers.slots[buffer.idx].resource, DXGI_FORMAT_R32_UINT,
        offset);
#endif
  } else {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 ctx->GLBuffers.slots[buffer.idx].resource);
  }
}

void Draw(RendererContext *ctx, uint32_t vertexCount, uint32_t startVertex) {
  if (ctx->GraphicsAPI == GraphicsAPIType::D3D11) {
#ifdef _WIN32
    ctx->D3DContext->Draw(vertexCount, startVertex);
#endif
  } else {
    glDrawArrays(GL_TRIANGLES, startVertex, vertexCount);
  }
}

void DrawIndexed(RendererContext *ctx, uint32_t indexCount, uint32_t startIndex,
                 int32_t baseVertex) {
  if (ctx->GraphicsAPI == GraphicsAPIType::D3D11) {
#ifdef _WIN32
    ctx->D3DContext->DrawIndexed(indexCount, startIndex, baseVertex);
#endif
  } else {
    glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT,
                             (void *)(uintptr_t)startIndex, baseVertex);
  }
}

#ifdef _WIN32
ID3D11Buffer *GetNativeD3D11Buffer(RendererContext *ctx, BufferHandle handle) {
  if (handle == INVALID_BUFFER)
    return nullptr;
  return ctx->D3DBuffers.slots[handle.idx].resource;
}
ID3D11Device *GetNativeD3D11Device(RendererContext *ctx) {
  return ctx->D3DDevice.Get();
}
#endif
unsigned int GetNativeGLBuffer(RendererContext *ctx, BufferHandle handle) {
  if (handle == INVALID_BUFFER)
    return 0;
  return ctx->GLBuffers.slots[handle.idx].resource;
}

} // namespace CrossGLDX
