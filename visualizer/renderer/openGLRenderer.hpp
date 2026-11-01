#pragma once

#include <memory>

#include "renderer.hpp"

namespace triangles::visualizer {

class GlfwWindow;
class ShaderProgram;

class OpenGLRenderer final : public Renderer {
public:
    explicit OpenGLRenderer(GlfwWindow& window);
    ~OpenGLRenderer() override;

    OpenGLRenderer(const OpenGLRenderer&) = delete;
    OpenGLRenderer& operator=(const OpenGLRenderer&) = delete;

    void render(const Frame& frame) override;

private:
    void releaseResources() noexcept;

    void beginFrame();
    void renderView(const RenderView& view, FramebufferSize framebufferSize);
    FramebufferSize prepareViewport(const NormalizedViewport& viewport,
                                    FramebufferSize framebufferSize);
    void drawGeometry(const Geometry& geometry);
    void endFrame();

    GlfwWindow& window_;

    std::unique_ptr<ShaderProgram> shaderProgram_;
    unsigned int vertexArray_ = 0;
    unsigned int vertexBuffer_ = 0;

    int modelViewProjectionUniformLocation_ = -1;
    int materialColorUniformLocation_ = -1;
};

} // namespace triangles::visualizer
