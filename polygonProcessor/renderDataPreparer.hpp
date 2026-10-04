#pragma once

#include <array>
#include <cstddef>
#include <vector>

#include "intersectionData.hpp"
#include "../render/renderData.hpp"

namespace HW3D {

struct RenderStyle {
    std::array<float, 4> polyColor {0.7f, 0.7f, 0.7f, 1.0f};
    std::array<float, 4> intersectingPolyColor {1.0f, 0.0f, 0.0f, 1.0f};
    std::array<float, 4> intersectionColor {1.0f, 0.8f, 0.0f, 1.0f};
};

// Adapts domain geometry to the renderer's fixed 3D format.
template <std::size_t DimWorld, std::size_t DimPoly>
class RenderDataPreparer {
public:
    // objects: triangulated polygons with normals; intersecting polygons are red.
    // highlights: Point -> Points, Segment -> Lines, Polygon -> Triangles.
    // Also emit every intersection vertex as Points in intersectionColor.
    // Polygon intersections require triangulation, not just copying their vertices.
    RenderData prepare(const std::vector<Poly<DimPoly>>& polys,
                       const IntersectionData& intersections,
                       const RenderStyle& style) const;
};

} // namespace HW3D
