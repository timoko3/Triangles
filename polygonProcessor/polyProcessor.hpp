#pragma once

#include <cstddef>

#include "initPolysData.hpp"
#include "intersectionData.hpp"
#include "polyWorld.hpp"
#include "renderDataPreparer.hpp"
#include "../render/renderer.hpp"

namespace HW3D {

template <std::size_t DimWorld, std::size_t DimPoly>
class PolyProcessor {
private:
    PolyWorld<DimWorld, DimPoly> world_;
    Renderer renderer_;
    RenderStyle renderStyle_;

public:
    PolyProcessor(const InitPolysData& initData);

    const IntersectionData& calculateIntersections() {
        return world_.calculateIntersections();
    }

    void drawPolys() {
        RenderDataPreparer<DimWorld, DimPoly> preparer;
        RenderData data = preparer.prepare(world_.polys(),
                                           world_.intersectionData(),
                                           renderStyle_);
        renderer_.draw(data);
    }
};

} // namespace HW3D
