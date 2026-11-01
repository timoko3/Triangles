#include "openGLRenderDataPreparer.hpp"

#include <stdexcept>

namespace triangles::visualizer {

GLenum primitiveTopology(Topology topology) {
    switch (topology) {
        case Topology::Points:
            return GL_POINTS;
        case Topology::Lines:
            return GL_LINES;
        case Topology::Triangles:
            return GL_TRIANGLES;
        default:
            throw std::invalid_argument("Unknown geometry topology");
    }
}

} // namespace triangles::visualizer
