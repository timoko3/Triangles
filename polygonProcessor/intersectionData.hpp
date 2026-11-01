#pragma once

#include <cstddef>
#include <vector>

namespace triangles {

class Point;

struct PolyIntersection {
    enum class Kind { Point, Segment, Polygon };

    Kind kind;
    // One point, two segment endpoints, or polygon vertices in boundary order.
    // The polygon's first vertex is not repeated at the end.
    std::vector<Point> points;
};

struct IntersectionData {
    struct Entry {
        // Indices in PolyWorld::polys(); each pair is stored once, first < second.
        std::size_t firstPolyIndex;
        std::size_t secondPolyIndex;
        PolyIntersection intersection;
    };

    // Only intersecting pairs. Empty means no intersections.
    // A collection of results, not their geometric union.
    std::vector<Entry> intersections;
};

} // namespace triangles
