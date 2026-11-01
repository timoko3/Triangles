#pragma once

#include <utility>
#include <vector>

#include "renderView.hpp"
#include "visualizerBackend.hpp"

namespace triangles::visualizer {

class Viewer {
private:
    ViewerBackend ViewerBackend_;
    std::vector<RenderView> views_;

public:
    Viewer() = default;
    explicit Viewer(ViewerBackend backend) : ViewerBackend_(std::move(backend)) {}

    void addView(const Scene& scene, CameraId camera,
                 NormalizedViewport viewport = {});

    void addBackend(ViewerBackend backend) {
        ViewerBackend_.renderer.reset();
        ViewerBackend_ = std::move(backend);
    }

    void run();
    void drawFrame();
};

} // namespace triangles::visualizer
