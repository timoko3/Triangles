#pragma once

#include <compare>
#include <cstdint>

#include "geometry.hpp"
#include "material.hpp"
#include "transform.hpp"

namespace triangles::visualizer {

struct ObjectId {
    std::uint32_t value;

    auto operator<=>(const ObjectId&) const = default;
};

struct SceneObject {
    Geometry geometry;
    Transform transform;
    Material material;
};

}
