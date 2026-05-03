#ifndef GLDX_RENDERER_HANDLE_H
#define GLDX_RENDERER_HANDLE_H

#pragma once
#include <stdint.h>

namespace CrossGLDX {

template <typename T> struct Handle {
  uint32_t idx = 0xFFFFFFFF;
  uint32_t generation = 0;

  bool empty() const { return idx != 0xFFFFFFFF; }

  bool operator==(const Handle &other) const { return idx == other.idx; }

  bool operator!=(const Handle &other) const { return idx != other.idx; }
};

struct BufferTag {};
struct ShaderTag {};
struct TextureTag {};
struct PipelineTag {};
struct SamplerTag {};

// Lightweight opaque handle used to reference a GPU buffer resource
using BufferHandle = Handle<BufferTag>;
// Lightweight opaque handle used to reference a compiled GPU shader
using ShaderHandle = Handle<ShaderTag>;
// Lightweight opaque handle used to reference a GPU texture resource
using TextureHandle = Handle<TextureTag>;
// Lightweight opaque handle used to reference a GPU pipeline state object which
// encapsulates shaders and render state configuration.
using PipelineHandle = Handle<PipelineTag>;
// Lightweight opaque handle used to reference a sampler state object
using SamplerHandle = Handle<SamplerTag>;

// Compile time constant symbols for invalid states of all handles.
constexpr BufferHandle INVALID_BUFFER = {0xFFFFFFFF};
// Compile time constant symbols for invalid states of all handles.
constexpr ShaderHandle INVALID_SHADER = {0xFFFFFFFF};
// Compile time constant symbols for invalid states of all handles.
constexpr TextureHandle INVALID_TEXTURE = {0xFFFFFFFF};
// Compile time constant symbols for invalid states of all handles.
constexpr PipelineHandle INVALID_PIPELINE = {0xFFFFFFFF};
// Compile time constant symbols for invalid states of all handles.
constexpr SamplerHandle INVALID_SAMPLER = {0xFFFFFFFF};

} // namespace CrossGLDX

#endif
