#pragma once

#include <memory>

#include "backendSettings.hpp"
#include "window.hpp"

struct GLFWwindow;

namespace triangles::visualizer {

// Construct, use and destroy GLFW windows on the main thread.
class GlfwWindow final : public Window {
public:
    GlfwWindow(const WindowSettings& settings, GraphicsApi graphics);
    ~GlfwWindow() override;

    GlfwWindow(const GlfwWindow&) = delete;
    GlfwWindow& operator=(const GlfwWindow&) = delete;

    void pollEvents() override;
    bool shouldClose() const override;
    FramebufferSize framebufferSize() const override;

    GLFWwindow* getWindow() const noexcept;

private:
    struct Runtime;
    static std::shared_ptr<Runtime> acquireRuntime();

    std::shared_ptr<Runtime> runtime_;
    GLFWwindow* window_ = nullptr;
};

} // namespace triangles::visualizer
