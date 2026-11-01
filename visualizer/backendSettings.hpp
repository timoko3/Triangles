#pragma once

#include <cstdint>
#include <string>

namespace triangles::visualizer {

enum class GraphicsApi {
    OpenGL,
    Vulkan
};

enum class WindowLibrary {
    GLFW
};

struct WindowSettings {
    std::string title = "Triangles";
    std::uint32_t width = 1280;
    std::uint32_t height = 720;
    bool resizable = true;
};

struct BackendSettings {
    GraphicsApi graphics = GraphicsApi::OpenGL;
    WindowLibrary windowLibrary = WindowLibrary::GLFW;
    WindowSettings window;
};

}