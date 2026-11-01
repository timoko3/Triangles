#pragma once

#include <array>
#include <glm/fwd.hpp>

namespace triangles::visualizer {

class ShaderProgram {
public:
    ShaderProgram(const char* vertexSource, const char* fragmentSource);
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void bind() const;
    static void unbind();

    int uniformLocation(const char* name) const;
    void setUniform(int location, const glm::mat4& value) const;
    void setUniform(int location, const std::array<float, 4>& value) const;

private:
    unsigned int program_ = 0;
};

} // namespace triangles::visualizer
