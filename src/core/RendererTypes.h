#ifndef CG_RENDERER_TYPES_H
#define CG_RENDERER_TYPES_H

#pragma once
#include <stdint.h>

namespace CrossGLDX {

    enum class BufferType {
        Vertex,
        Index,
        Constant
    };

    enum class BufferUsage {
        Static,
        Dynamic
    };

}

#endif
