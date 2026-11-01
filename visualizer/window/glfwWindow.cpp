#include "glfwWindow.hpp"

#include <limits>
#include <stdexcept>

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

namespace triangles::visualizer {

struct GlfwWindow::Runtime {
    Runtime() {
        if (glfwInit() != GLFW_TRUE) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
    }

    ~Runtime() { glfwTerminate(); }
};

std::shared_ptr<GlfwWindow::Runtime> GlfwWindow::acquireRuntime() {
    static std::weak_ptr<Runtime> current;
    auto runtime = current.lock();

    if (!runtime) {
        runtime = std::make_shared<Runtime>();
        current = runtime;
    }

    return runtime;
}

GlfwWindow::GlfwWindow(const WindowSettings& settings, GraphicsApi graphics) {
    const auto maxSize = static_cast<std::uint32_t>(std::numeric_limits<int>::max());
    if (settings.width == 0 || settings.height == 0 || settings.width > maxSize ||
        settings.height > maxSize) {
        throw std::invalid_argument(
            "Window dimensions must be positive and fit in int");
    }

    runtime_ = acquireRuntime();

    glfwDefaultWindowHints();
    switch (graphics) {
        case GraphicsApi::OpenGL:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
            break;
        case GraphicsApi::Vulkan:
            throw std::invalid_argument("not implemented yet");
            break;
        default:
            throw std::invalid_argument("Unknown graphics API");
    }

    glfwWindowHint(GLFW_RESIZABLE, settings.resizable ? GLFW_TRUE : GLFW_FALSE);

    window_ = glfwCreateWindow(static_cast<int>(settings.width),
                               static_cast<int>(settings.height),
                               settings.title.c_str(),
                               nullptr,
                               nullptr);
    if (!window_) {
        throw std::runtime_error(
            "Failed to create GLFW window for the selected graphics API");
    }
}

GlfwWindow::~GlfwWindow() {
    glfwDestroyWindow(window_);
}

void GlfwWindow::pollEvents() {
    glfwPollEvents();
}

bool GlfwWindow::shouldClose() const {
    return glfwWindowShouldClose(window_) == GLFW_TRUE;
}

FramebufferSize GlfwWindow::framebufferSize() const {
    FramebufferSize size{};
    glfwGetFramebufferSize(window_, &size.width, &size.height);

    return size;
}

GLFWwindow* GlfwWindow::getWindow() const noexcept {
    return window_;
}

} // namespace triangles::visualizer
