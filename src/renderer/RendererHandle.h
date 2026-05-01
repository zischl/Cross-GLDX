#ifndef GLDX_RENDERER_HANDLE_H
#define GLDX_RENDERER_HANDLE_H

#pragma once
#include <stdint.h>

namespace CrossGLDX {

    
    /// Lightweight opaque handle used to reference a GPU buffer resource
    struct BufferHandle { uint32_t idx; };

    /// Lightweight opaque handle used to reference a compiled GPU shader
    struct ShaderHandle { uint32_t idx; };

    /// Lightweight opaque handle used to reference a GPU texture resource
    struct TextureHandle { uint32_t idx; };

    /// Lightweight opaque handle used to reference a pipeline state object which encapsulates shaders and render state configuration.
    struct PipelineHandle { uint32_t idx; };

    /// Lightweight opaque handle used to reference a sampler state object
    struct SamplerHandle { uint32_t idx; };


    // Compile time constant symbols for invalid states of all handles.
    constexpr BufferHandle   INVALID_BUFFER   = { 0xFFFFFFFF };
    // Compile time constant symbols for invalid states of all handles.
    constexpr ShaderHandle   INVALID_SHADER   = { 0xFFFFFFFF };
    // Compile time constant symbols for invalid states of all handles.
    constexpr TextureHandle  INVALID_TEXTURE  = { 0xFFFFFFFF };
    // Compile time constant symbols for invalid states of all handles.
    constexpr PipelineHandle INVALID_PIPELINE = { 0xFFFFFFFF };
    // Compile time constant symbols for invalid states of all handles.
    constexpr SamplerHandle  INVALID_SAMPLER  = { 0xFFFFFFFF };


    // Compares two texture handles based on the operator for either equality or inequality.
    inline bool operator==(BufferHandle a, BufferHandle b) { return a.idx == b.idx; }
    // Compares two texture handles based on the operator for either equality or inequality.
    inline bool operator!=(BufferHandle a, BufferHandle b) { return a.idx != b.idx; }

    // Compares two texture handles based on the operator for either equality or inequality.
    inline bool operator==(ShaderHandle a, ShaderHandle b) { return a.idx == b.idx; }
    // Compares two texture handles based on the operator for either equality or inequality.
    inline bool operator!=(ShaderHandle a, ShaderHandle b) { return a.idx != b.idx; }

    // Compares two texture handles based on the operator for either equality or inequality.
    inline bool operator==(TextureHandle a, TextureHandle b) { return a.idx == b.idx; }
    // Compares two texture handles based on the operator for either equality or inequality.
    inline bool operator!=(TextureHandle a, TextureHandle b) { return a.idx != b.idx; }

    // Compares two texture handles based on the operator for either equality or inequality.
    inline bool operator==(PipelineHandle a, PipelineHandle b) { return a.idx == b.idx; }
    // Compares two texture handles based on the operator for either equality or inequality.
    inline bool operator!=(PipelineHandle a, PipelineHandle b) { return a.idx != b.idx; }

}

#endif
