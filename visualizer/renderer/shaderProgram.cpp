#include "shaderProgram.hpp"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <string>

namespace triangles::visualizer {
namespace {

GLuint compileShader(GLenum type, const char* source) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        char log[2048]{};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        glDeleteShader(shader);

        throw std::runtime_error(std::string("Shader compilation failed: ") + log);
    }

    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource) {
    const GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexSource);

    GLuint fragment;
    try {
        fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    } catch (...) {
        glDeleteShader(vertex);

        throw;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        char log[2048]{};
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        glDeleteProgram(program);

        throw std::runtime_error(std::string("Shader linking failed: ") + log);
    }

    return program;
}

} // namespace

ShaderProgram::ShaderProgram(const char* vertexSource, const char* fragmentSource)
    : program_(createProgram(vertexSource, fragmentSource)) {}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(program_);
}

void ShaderProgram::bind() const {
    glUseProgram(program_);
}

void ShaderProgram::unbind() {
    glUseProgram(0);
}

int ShaderProgram::uniformLocation(const char* name) const {
    return glGetUniformLocation(program_, name);
}

void ShaderProgram::setUniform(int location, const glm::mat4& value) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(int location,
                               const std::array<float, 4>& value) const {
    glUniform4fv(location, 1, value.data());
}

} // namespace triangles::visualizer
