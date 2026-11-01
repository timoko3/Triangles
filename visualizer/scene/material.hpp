#pragma once

#include <array>

namespace triangles::visualizer {

struct Material {
    std::array<float, 4> color{0.7f, 0.7f, 0.7f, 1.0f};
};

}
