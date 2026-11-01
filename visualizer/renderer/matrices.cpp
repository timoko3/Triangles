#include "matrices.hpp"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <stdexcept>

namespace triangles::visualizer {
namespace {

glm::vec3 vector(const std::array<float, 3>& value) {
    return {value[0], value[1], value[2]};
}

glm::quat quaternion(const Quaternion& value) {
    const double length = std::hypot(
        std::hypot(static_cast<double>(value.x), static_cast<double>(value.y)),
        std::hypot(static_cast<double>(value.z), static_cast<double>(value.w)));
    if (!std::isfinite(length) || length == 0) {
        throw std::invalid_argument("Rotation must be a finite nonzero quaternion");
    }

    return {static_cast<float>(value.w / length),
            static_cast<float>(value.x / length),
            static_cast<float>(value.y / length),
            static_cast<float>(value.z / length)};
}

} // namespace

glm::mat4 modelMatrix(const Transform& transform) {
    return glm::translate(glm::mat4(1), vector(transform.position)) *
           glm::mat4_cast(quaternion(transform.rotation)) *
           glm::scale(glm::mat4(1), vector(transform.scale));
}

glm::mat4 viewMatrix(const Camera& camera) {
    return glm::mat4_cast(glm::conjugate(quaternion(camera.rotation()))) *
           glm::translate(glm::mat4(1), -vector(camera.position()));
}

glm::mat4 projectionMatrix(const Camera& camera, float aspect) {
    const auto& settings = camera.settings();
    if (settings.type == Camera::Type::Perspective) {
        return glm::perspective(settings.verticalFovRadians,
                                aspect,
                                settings.nearPlane,
                                settings.farPlane);
    }

    const float halfHeight = settings.verticalSize / 2;
    const float halfWidth = halfHeight * aspect;

    return glm::ortho(-halfWidth,
                      halfWidth,
                      -halfHeight,
                      halfHeight,
                      settings.nearPlane,
                      settings.farPlane);
}

} // namespace triangles::visualizer
