#include "visualizer.hpp"

#include <chrono>
#include <cmath>
#include <stdexcept>
#include <thread>

namespace triangles::visualizer {

void Viewer::addView(const Scene& scene, CameraId camera,
                     NormalizedViewport viewport) {
    scene.camera(camera);

    if (!std::isfinite(viewport.x) || !std::isfinite(viewport.y) ||
        !std::isfinite(viewport.width) || !std::isfinite(viewport.height) ||
        viewport.x < 0 || viewport.y < 0 || viewport.width <= 0 ||
        viewport.height <= 0 || viewport.x + viewport.width > 1 ||
        viewport.y + viewport.height > 1) {
        throw std::invalid_argument(
            "Viewport must have positive dimensions and fit inside [0, 1]");
    }

    for (const auto& view : views_) {
        const auto& other = view.viewport;

        if (viewport.x < other.x + other.width &&
            other.x < viewport.x + viewport.width &&
            viewport.y < other.y + other.height &&
            other.y < viewport.y + viewport.height) {
            throw std::invalid_argument("Viewports must not overlap");
        }
    }

    views_.push_back({scene, camera, viewport});
}

void Viewer::drawFrame() {
    if (!ViewerBackend_.window || !ViewerBackend_.renderer) {
        throw std::logic_error("Drawing requires a window and a renderer");
    }

    const auto size = ViewerBackend_.window->framebufferSize();
    if (size.width <= 0 || size.height <= 0) {
        return;
    }

    const Frame frame{views_, size};
    ViewerBackend_.renderer->render(frame);
}

void Viewer::run() {
    if (!ViewerBackend_.window || !ViewerBackend_.renderer) {
        throw std::logic_error("Running requires a window and a renderer");
    }

    auto& window = *ViewerBackend_.window;

    while (!window.shouldClose()) {
        window.pollEvents();

        if (window.shouldClose()) {
            break;
        }

        drawFrame();

        const auto size = window.framebufferSize();
        if (size.width <= 0 || size.height <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
}

} // namespace triangles::visualizer
