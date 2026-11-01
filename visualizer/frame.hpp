#pragma once

#include <span>

#include "renderView.hpp"
#include "window/window.hpp"

namespace triangles::visualizer {

struct Frame {
    std::span<const RenderView> views;
    FramebufferSize framebufferSize;
};

}
