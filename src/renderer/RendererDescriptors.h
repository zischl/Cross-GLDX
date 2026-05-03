#ifndef GLDX_RENDERER_DESCRIPTORS_H
#define GLDX_RENDERER_DESCRIPTORS_H

#pragma once
#include <stdint.h>

namespace CrossGLDX {

enum class BufferType : uint8_t { Vertex, Index, Constant, Structured };

enum class BufferAccess : uint8_t { GPUOnly, CPUWrite, CPUReadWrite };

struct BufferDesc {
  BufferType type = BufferType::Vertex;
  BufferAccess access = BufferAccess::GPUOnly;
  size_t size = 0;
  const char *debugName = nullptr;
};

} // namespace CrossGLDX

#endif
