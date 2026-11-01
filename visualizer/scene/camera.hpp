#pragma once

#include <cstdint>
#include <compare>
#include <numbers>

#include "transform.hpp"

namespace triangles::visualizer {

struct CameraId {
    std::uint32_t value;

    auto operator<=>(const CameraId&) const = default;
};

class Camera {
public:
    enum class Type {
        Perspective,
        Orthographic
    };

    struct Settings {
        Type type = Type::Perspective;
        float verticalFovRadians = std::numbers::pi_v<float> / 4.0f;
        float verticalSize = 10.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
    };

    Camera() = default;
    explicit Camera(Settings settings);

    void setSettings(Settings settings);
    const Settings& settings() const noexcept;

    void setPosition(std::array<float, 3> position);
    void setRotation(Quaternion rotation);

    const std::array<float, 3>& position() const noexcept;
    const Quaternion& rotation() const noexcept;

private:
    Settings settings_;
    std::array<float, 3> position_{};
    Quaternion rotation_;
};

}
