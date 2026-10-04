#pragma once

#include <cstddef>
#include <vector>

#include "initPolysData.hpp"
#include "intersectionData.hpp"
#include "polyCalculator.hpp"

namespace HW3D {

template <std::size_t DimWorld, std::size_t DimPoly>
class PolyWorld {
private:
    std::vector<Poly<DimPoly>> polys_;
    IntersectionData intersectionData_;

public:
    PolyWorld(const InitPolysData& initData);

    const IntersectionData& calculateIntersections() {
        PolyCalculator<DimPoly> calculator;
        intersectionData_ = calculator.calculateIntersections(polys_);
        return intersectionData_;
    }

    const std::vector<Poly<DimPoly>>& polys() const {
        return polys_;
    }

    const IntersectionData& intersectionData() const {
        return intersectionData_;
    }
};

} // namespace HW3D
