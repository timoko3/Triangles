#pragma once 

#include <memory>
#include "window.hpp"
#include "renderer/renderer.hpp"

namespace triangles::visualizer {

struct ViewerBackend {
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
};

}
