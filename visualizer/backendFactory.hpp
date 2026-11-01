#pragma once 

#include "backendSettings.hpp"
#include "viewerBackend.hpp"

namespace triangles::visualizer {

class BackendFactory {
public:
    static ViewerBackend create(const BackendSettings& settings);
};

}
