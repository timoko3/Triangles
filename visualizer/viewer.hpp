#pragma once 

#include <utility>
#include "viewerBackend.hpp"

namespace triangles::visualizer {

class Scene;
struct CameraId;

class Viewer {
private:
    ViewerBackend ViewerBackend_;
public:
    Viewer() = default;
    explicit Viewer(ViewerBackend backend) : ViewerBackend_(std::move(backend)) {}

    void addView(const Scene& scene, CameraId camera);

    void addBackend(ViewerBackend backend) {
        ViewerBackend_.renderer.reset();
        ViewerBackend_ = std::move(backend);
    }

    void run();
    void drawFrame();
};

}
