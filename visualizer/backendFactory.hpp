#pragma once

#include "backendSettings.hpp"
#include "visualizerBackend.hpp"

namespace triangles::visualizer {

class BackendFactory {
public:
    static ViewerBackend create(const BackendSettings& settings);
};

} // namespace triangles::visualizer
