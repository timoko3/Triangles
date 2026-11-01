#include "openGLRenderer.hpp"

#include <GLFW/glfw3.h>
#include <cmath>
#include <cstddef>
#include <glad/gl.h>
#include <limits>
#include <stdexcept>

#include "../window/glfwWindow.hpp"
#include "matrices.hpp"
#include "openGLRenderDataPreparer.hpp"
#include "shaderProgram.hpp"
#include "shaders/flatColor.hpp"

namespace triangles::visualizer {

OpenGLRenderer::OpenGLRenderer(GlfwWindow& window) : window_(window) {
    glfwMakeContextCurrent(window_.getWindow());
    if (gladLoadGL(glfwGetProcAddress) == 0) {
        throw std::runtime_error("Failed to load OpenGL");
    }

    glfwSwapInterval(1);

    try {
        shaderProgram_ = std::make_unique<ShaderProgram>(shaders::flatColorVertex,
                                                         shaders::flatColorFragment);
        modelViewProjectionUniformLocation_ =
            shaderProgram_->uniformLocation("modelViewProjection");
        materialColorUniformLocation_ =
            shaderProgram_->uniformLocation("materialColor");

        glGenVertexArrays(1, &vertexArray_);
        glGenBuffers(1, &vertexBuffer_);

        glBindVertexArray(vertexArray_);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            reinterpret_cast<const void*>(offsetof(Vertex, position)));
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (glGetError() != GL_NO_ERROR) {
            throw std::runtime_error(
                "Failed to initialize OpenGL renderer resources");
        }
    } catch (...) {
        releaseResources();

        throw;
    }
}

OpenGLRenderer::~OpenGLRenderer() {
    glfwMakeContextCurrent(window_.getWindow());
    releaseResources();
}

void OpenGLRenderer::releaseResources() noexcept {
    glDeleteBuffers(1, &vertexBuffer_);
    glDeleteVertexArrays(1, &vertexArray_);
    shaderProgram_.reset();
}

void OpenGLRenderer::render(const Frame& frame) {
    const auto size = frame.framebufferSize;
    if (size.width <= 0 || size.height <= 0) {
        return;
    }

    beginFrame();

    for (const auto& view : frame.views) {
        renderView(view, size);
    }

    endFrame();
}

void OpenGLRenderer::beginFrame() {
    glfwMakeContextCurrent(window_.getWindow());

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    glClearColor(0.08f, 0.1f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram_->bind();
    glBindVertexArray(vertexArray_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glEnable(GL_SCISSOR_TEST);
}

FramebufferSize OpenGLRenderer::prepareViewport(const NormalizedViewport& viewport,
                                                FramebufferSize framebufferSize) {
    const int left =
        static_cast<int>(std::lround(viewport.x * framebufferSize.width));
    const int right = static_cast<int>(
        std::lround((viewport.x + viewport.width) * framebufferSize.width));
    const int top =
        static_cast<int>(std::lround(viewport.y * framebufferSize.height));
    const int bottom = static_cast<int>(
        std::lround((viewport.y + viewport.height) * framebufferSize.height));

    const FramebufferSize viewportSize{right - left, bottom - top};
    if (viewportSize.width <= 0 || viewportSize.height <= 0) {
        return {};
    }

    const int openGLBottom = framebufferSize.height - bottom;
    glViewport(left, openGLBottom, viewportSize.width, viewportSize.height);
    glScissor(left, openGLBottom, viewportSize.width, viewportSize.height);

    return viewportSize;
}

void OpenGLRenderer::renderView(const RenderView& view,
                                FramebufferSize framebufferSize) {
    const auto viewportSize = prepareViewport(view.viewport, framebufferSize);
    if (viewportSize.width <= 0 || viewportSize.height <= 0) {
        return;
    }

    const auto& camera = view.scene.camera(view.camera);
    const float aspect =
        static_cast<float>(viewportSize.width) / viewportSize.height;
    const auto viewProjection =
        projectionMatrix(camera, aspect) * viewMatrix(camera);

    for (const auto& [id, object] : view.scene.objects()) {
        if (object.geometry.vertices.empty()) {
            continue;
        }

        const auto modelViewProjection =
            viewProjection * modelMatrix(object.transform);
        shaderProgram_->setUniform(modelViewProjectionUniformLocation_,
                                   modelViewProjection);
        shaderProgram_->setUniform(materialColorUniformLocation_,
                                   object.material.color);

        drawGeometry(object.geometry);
    }
}

void OpenGLRenderer::drawGeometry(const Geometry& geometry) {
    if (geometry.vertices.size() >
            static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()) ||
        geometry.vertices.size() >
            static_cast<std::size_t>(std::numeric_limits<GLsizeiptr>::max()) /
                sizeof(Vertex)) {
        throw std::length_error("Too many vertices in geometry");
    }

    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(geometry.vertices.size() * sizeof(Vertex)),
                 geometry.vertices.data(),
                 GL_STREAM_DRAW);

    glDrawArrays(primitiveTopology(geometry.topology),
                 0,
                 static_cast<GLsizei>(geometry.vertices.size()));
}

void OpenGLRenderer::endFrame() {
    glDisable(GL_SCISSOR_TEST);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    ShaderProgram::unbind();

    glfwSwapBuffers(window_.getWindow());
}

} // namespace triangles::visualizer
