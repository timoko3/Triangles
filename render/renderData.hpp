#pragma once

#include <array>
#include <vector>

namespace HW3D {

struct RenderVertex {
    std::array<float, 3> position;
    // Used for triangle lighting; ignored for points and lines.
    std::array<float, 3> normal;
};

struct RenderPrimitive {
    enum class Topology { Points, Lines, Triangles };

    Topology topology;
    std::array<float, 4> color;
    // Independent vertices: 1 per point, 2 per line, 3 per triangle.
    std::vector<RenderVertex> vertices;
};

struct RenderData {
    std::vector<RenderPrimitive> objects;
    // A separate overlay pass, independent of the domain's IntersectionData.
    std::vector<RenderPrimitive> highlights;
};

} // namespace HW3D
