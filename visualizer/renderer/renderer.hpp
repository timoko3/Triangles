#pragma once

#include "renderData.hpp"

namespace triangles::visualizer {

class Renderer {
public:
    virtual ~Renderer() = default;
    // Render objects first, then highlights without lighting.
    // The highlight pass must handle depth conflicts and visible point sizing.
    virtual void draw(const RenderData& data) = 0;
};

} // namespace triangles::visualizer
