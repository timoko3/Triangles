#pragma once

#include <cstdint>
#include <map>

#include "camera.hpp"
#include "sceneObject.hpp"

namespace triangles::visualizer {

class Scene {
public:
    ObjectId addObject(SceneObject object);
    CameraId addCamera(Camera camera);

    SceneObject& object(ObjectId id);
    const SceneObject& object(ObjectId id) const;

    Camera& camera(CameraId id);
    const Camera& camera(CameraId id) const;

    void removeObject(ObjectId id);
    void removeCamera(CameraId id);

    const std::map<ObjectId, SceneObject>& objects() const noexcept;

private:
    std::map<ObjectId, SceneObject> objects_;
    std::map<CameraId, Camera> cameras_;
    std::uint64_t nextObjectId_ = 0;
    std::uint64_t nextCameraId_ = 0;
};

} // namespace triangles::visualizer
