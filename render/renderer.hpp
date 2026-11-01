#pragma once

#include "renderData.hpp"

namespace triangles {

// Interface sketch. Camera, lighting, scene and GPU resources are renderer state.
// The concrete state and OpenGL implementation will be defined separately.
class Renderer {
public:
    // Render objects first, then highlights without lighting.
    // The highlight pass must handle depth conflicts and visible point sizing.
    void draw(const RenderData& data);
};

} // namespace triangles
