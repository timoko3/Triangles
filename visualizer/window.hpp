#pragma once

namespace triangles::visualizer {

struct FramebufferSize {
    int width;
    int height;
};

class Window {
public:
    virtual ~Window() = default;

    virtual void pollEvents() = 0;
    virtual bool shouldClose() const = 0;
    virtual FramebufferSize framebufferSize() const = 0;
};

}
