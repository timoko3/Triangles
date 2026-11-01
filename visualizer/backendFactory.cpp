#include "backendFactory.hpp"
#include "glfwWindow.hpp"

#include <stdexcept>

namespace triangles::visualizer {

ViewerBackend BackendFactory::create(const BackendSettings& settings) {
    switch (settings.windowLibrary) {
        case WindowLibrary::GLFW:
            return {std::make_unique<GlfwWindow>(settings.window, settings.graphics), nullptr};
        default:
            throw std::invalid_argument("Unknown window library");
    }
}

} // namespace triangles::visualizer
