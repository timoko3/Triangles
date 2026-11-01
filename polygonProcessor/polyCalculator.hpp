#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "intersectionData.hpp"

namespace triangles {

// template <std::size_t DimPoly>
// class Poly;

template <std::size_t DimPoly>
class PolyCalculator {
public:
    // Collect all intersecting pairs and their intersection geometry.
    IntersectionData calculateIntersections(const std::vector<Poly<DimPoly>>& polys);

private:
    // std::nullopt means the pair does not intersect.
    std::optional<PolyIntersection> IntersectsPoly(const Poly<DimPoly>& first,
                                                   const Poly<DimPoly>& second);
};

} // namespace triangles
