#pragma once

#include <array>
#include <vector>

namespace triangles::visualizer {

enum class Topology {
    Points,
    Lines,
    Triangles
};

struct Vertex {
    std::array<float, 3> position{};
    std::array<float, 3> normal{};
};

struct Geometry {
    Topology topology = Topology::Triangles;
    std::vector<Vertex> vertices;
};

}
