#pragma once

#include <glm/mat4x4.hpp>

#include "../scene/camera.hpp"
#include "../scene/transform.hpp"

namespace triangles::visualizer {

glm::mat4 modelMatrix(const Transform& transform);
glm::mat4 viewMatrix(const Camera& camera);
glm::mat4 projectionMatrix(const Camera& camera, float aspect);

} // namespace triangles::visualizer
