#include "backendFactory.hpp"

#include <stdexcept>

#include "renderer/openGLRenderer.hpp"
#include "window/glfwWindow.hpp"

namespace triangles::visualizer {

ViewerBackend BackendFactory::create(const BackendSettings& settings) {
    switch (settings.windowLibrary) {
        case WindowLibrary::GLFW: {
            if (settings.graphics != GraphicsApi::OpenGL) {
                throw std::invalid_argument(
                    "Only the OpenGL renderer is implemented");
            }

            auto window =
                std::make_unique<GlfwWindow>(settings.window, settings.graphics);
            auto renderer = std::make_unique<OpenGLRenderer>(*window);

            return {std::move(window), std::move(renderer)};
        }
        default:
            throw std::invalid_argument("Unknown window library");
    }
}

} // namespace triangles::visualizer
