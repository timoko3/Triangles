#include <array>
#include <cmath>
#include <exception>
#include <iostream>
#include <numbers>
#include <utility>

#include "visualizer/backendFactory.hpp"
#include "visualizer/visualizer.hpp"

namespace {

struct TriangleSettings {
    std::array<float, 3> position;
    float rotationDegrees;
    float scale;
    std::array<float, 4> color;
};

triangles::visualizer::Quaternion rotationAroundZ(float degrees) {
    const float halfAngle = degrees * std::numbers::pi_v<float> / 360.0f;

    return {0.0f, 0.0f, std::sin(halfAngle), std::cos(halfAngle)};
}

} // namespace

int main() {
    using namespace triangles::visualizer;

    try {
        Scene scene;

        const Geometry geometry{
            .topology = Topology::Triangles,
            .vertices = {{{-0.7f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                         {{0.7f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                         {{0.0f, 0.7f, 0.0f}, {0.0f, 0.0f, 1.0f}}}};

        const std::array<TriangleSettings, 10> triangles{
            {{{-3.6f, 1.3f, -0.8f}, -25.0f, 0.90f, {0.95f, 0.35f, 0.20f, 1.0f}},
             {{-1.8f, 1.1f, 0.3f}, 15.0f, 0.75f, {0.95f, 0.65f, 0.15f, 1.0f}},
             {{0.0f, 1.5f, -0.4f}, 45.0f, 1.00f, {0.65f, 0.85f, 0.20f, 1.0f}},
             {{1.8f, 1.2f, 0.7f}, -10.0f, 0.85f, {0.20f, 0.80f, 0.45f, 1.0f}},
             {{3.6f, 1.4f, -0.2f}, 80.0f, 0.95f, {0.15f, 0.75f, 0.80f, 1.0f}},
             {{-3.5f, -1.3f, 0.4f}, 150.0f, 0.80f, {0.25f, 0.55f, 0.95f, 1.0f}},
             {{-1.7f, -1.1f, -1.0f}, -45.0f, 1.10f, {0.45f, 0.35f, 0.95f, 1.0f}},
             {{0.2f, -1.5f, 0.6f}, 30.0f, 0.75f, {0.75f, 0.30f, 0.90f, 1.0f}},
             {{1.9f, -1.2f, -0.5f}, 110.0f, 1.00f, {0.95f, 0.35f, 0.65f, 1.0f}},
             {{3.6f, -1.4f, 0.1f}, -70.0f, 0.85f, {0.85f, 0.85f, 0.90f, 1.0f}}}};

        for (const auto& triangleSettings : triangles) {
            SceneObject triangle;
            triangle.geometry = geometry;
            triangle.transform.position = triangleSettings.position;
            triangle.transform.rotation =
                rotationAroundZ(triangleSettings.rotationDegrees);
            triangle.transform.scale = {triangleSettings.scale,
                                        triangleSettings.scale,
                                        triangleSettings.scale};
            triangle.material.color = triangleSettings.color;

            scene.addObject(std::move(triangle));
        }

        Camera camera;
        camera.setPosition({0.0f, 0.0f, 8.0f});
        const auto cameraId = scene.addCamera(std::move(camera));

        BackendSettings settings;
        settings.window.title = "Triangle viewer - 10 triangles";
        settings.window.width = 1280;
        settings.window.height = 720;

        Viewer viewer(BackendFactory::create(settings));
        viewer.addView(scene, cameraId);
        viewer.run();
    } catch (const std::exception& error) {
        std::cerr << "Failed to run the triangle example: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
