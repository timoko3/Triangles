#pragma once

#include "scene/camera.hpp"
#include "scene/scene.hpp"
#include "viewport.hpp"

namespace triangles::visualizer {

struct RenderView {
    const Scene& scene;
    CameraId camera;
    NormalizedViewport viewport;
};

}
