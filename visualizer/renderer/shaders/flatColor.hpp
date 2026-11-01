#pragma once

namespace triangles::visualizer::shaders {

inline constexpr char flatColorVertex[] = R"(
#version 330 core
layout(location = 0) in vec3 position;
uniform mat4 modelViewProjection;
void main() { gl_Position = modelViewProjection * vec4(position, 1.0); }
)";

inline constexpr char flatColorFragment[] = R"(
#version 330 core
uniform vec4 materialColor;
out vec4 fragmentColor;
void main() { fragmentColor = materialColor; }
)";

} // namespace triangles::visualizer::shaders
