#pragma once

#include <array>

namespace triangles::visualizer {

struct Quaternion {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;
};

struct Transform {
    std::array<float, 3> position{0.0f, 0.0f, 0.0f};
    Quaternion rotation;
    std::array<float, 3> scale{1.0f, 1.0f, 1.0f};
};

}
