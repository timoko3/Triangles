#include "scene.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

namespace triangles::visualizer {

const std::map<ObjectId, SceneObject>& Scene::objects() const noexcept {
    return objects_;
}

ObjectId Scene::addObject(SceneObject object) {
    if (nextObjectId_ > std::numeric_limits<std::uint32_t>::max()) {
        throw std::overflow_error("Scene object IDs are exhausted");
    }

    const ObjectId id{static_cast<std::uint32_t>(nextObjectId_)};
    objects_.emplace(id, std::move(object));
    ++nextObjectId_;
    return id;
}

CameraId Scene::addCamera(Camera camera) {
    if (nextCameraId_ > std::numeric_limits<std::uint32_t>::max()) {
        throw std::overflow_error("Scene camera IDs are exhausted");
    }

    const CameraId id{static_cast<std::uint32_t>(nextCameraId_)};
    cameras_.emplace(id, std::move(camera));
    ++nextCameraId_;
    return id;
}

SceneObject& Scene::object(ObjectId id) {
    return objects_.at(id);
}

const SceneObject& Scene::object(ObjectId id) const {
    return objects_.at(id);
}

Camera& Scene::camera(CameraId id) {
    return cameras_.at(id);
}

const Camera& Scene::camera(CameraId id) const {
    return cameras_.at(id);
}

void Scene::removeObject(ObjectId id) {
    if (objects_.erase(id) == 0) {
        throw std::out_of_range("Unknown scene object ID");
    }
}

void Scene::removeCamera(CameraId id) {
    if (cameras_.erase(id) == 0) {
        throw std::out_of_range("Unknown scene camera ID");
    }
}

} // namespace triangles::visualizer
