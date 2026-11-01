#pragma once

#include <memory>

#include "renderer/renderer.hpp"
#include "window/window.hpp"

namespace triangles::visualizer {

struct ViewerBackend {
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
};

} // namespace triangles::visualizer
