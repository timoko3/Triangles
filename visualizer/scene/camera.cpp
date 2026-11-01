#include "camera.hpp"

#include <cmath>
#include <stdexcept>

namespace triangles::visualizer {

Camera::Camera(Settings settings) {
    setSettings(settings);
}

void Camera::setSettings(Settings settings) {
    if (!std::isfinite(settings.nearPlane) || !std::isfinite(settings.farPlane) ||
        settings.nearPlane <= 0.0f || settings.farPlane <= settings.nearPlane) {
        throw std::invalid_argument(
            "Camera clipping planes must satisfy 0 < near < far");
    }

    switch (settings.type) {
        case Type::Perspective:
            if (!std::isfinite(settings.verticalFovRadians) ||
                settings.verticalFovRadians <= 0.0f ||
                settings.verticalFovRadians >= std::numbers::pi_v<float>) {
                throw std::invalid_argument(
                    "Camera vertical FOV must be between 0 and pi radians");
            }
            break;
        case Type::Orthographic:
            if (!std::isfinite(settings.verticalSize) ||
                settings.verticalSize <= 0.0f) {
                throw std::invalid_argument(
                    "Camera vertical size must be finite and positive");
            }
            break;
        default:
            throw std::invalid_argument("Unknown camera type");
    }

    settings_ = settings;
}

const Camera::Settings& Camera::settings() const noexcept {
    return settings_;
}

void Camera::setPosition(std::array<float, 3> position) {
    for (const float component : position) {
        if (!std::isfinite(component)) {
            throw std::invalid_argument("Camera position must be finite");
        }
    }

    position_ = position;
}

void Camera::setRotation(Quaternion rotation) {
    if (!std::isfinite(rotation.x) || !std::isfinite(rotation.y) ||
        !std::isfinite(rotation.z) || !std::isfinite(rotation.w)) {
        throw std::invalid_argument("Camera rotation must be finite");
    }

    const double length = std::hypot(
        std::hypot(static_cast<double>(rotation.x), static_cast<double>(rotation.y)),
        std::hypot(static_cast<double>(rotation.z),
                   static_cast<double>(rotation.w)));
    if (length == 0.0) {
        throw std::invalid_argument("Camera rotation must be a nonzero quaternion");
    }

    rotation_ = {static_cast<float>(rotation.x / length),
                 static_cast<float>(rotation.y / length),
                 static_cast<float>(rotation.z / length),
                 static_cast<float>(rotation.w / length)};
}

const std::array<float, 3>& Camera::position() const noexcept {
    return position_;
}

const Quaternion& Camera::rotation() const noexcept {
    return rotation_;
}

} // namespace triangles::visualizer
