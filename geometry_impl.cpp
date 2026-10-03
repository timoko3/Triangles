module;

#include <cmath>
#include <stdexcept>

module geometry;

namespace geometry {

constexpr float EPSILON = 1e-9;

point_t::point_t(double x, double y, double z)
    : x_(x),
      y_(y),
      z_(z) {
    if (!(std::isfinite(x) &&
          std::isfinite(y) &&
          std::isfinite(z))) {
        throw std::invalid_argument("Point coordinates must be finite");
    }
}

bool
point_t::equal (const point_t &refPoint) const {
    return ((std::fabs(refPoint.x_ - x_) < EPSILON)&&
            (std::fabs(refPoint.y_ - y_) < EPSILON)&&
            (std::fabs(refPoint.z_ - z_) < EPSILON));
}

double
point_t::distance(const point_t &otherPoint) const {
    double dx = x_ - otherPoint_x;
    double dy = y_ - otherPoint_y;
    double dz = z_ - otherPoint_z;

    return std::hypot(dx, dy, dz);
};


vector_t::vector_t(const point_t &a, const point_t &b) {
    x_ = a.x_ - b.x_;
    y_ = a.y_ - b.y_;
    x_ = a.z_ - b.z_;
};

bool
vector_t::equal(const vector_t &refVector) const {
    return ((std::fabs(refVector.x_ - x_) < EPSILON)&&
            (std::fabs(refVector.y_ - y_) < EPSILON)&&
            (std::fabs(refVector.z_ - z_) < EPSILON));
};

bool
vector_t::isValid() const {
    return !(std::isnan(x_) || std::isnan(y_) || std::isnan(z_));
};

double
vector_t::length() const {
    return std::hypot(x_, y_, z_);
}

vector_t
vector_t::cross(const vector_t &otherVector) const {
    return vector_t(
        y_ * otherVector.z_ - z_ * otherVector.y_,
        z_ * otherVector.x_ - x_ * otherVector.z_,
        x_ * otherVector.y_ - y_ * otherVector.x_
    );
}

triangle_t::triangle_t(const point_t &firstPoint,
                       const point_t &secondPoint,
                       const point_t &thirdPoint)
    : a_(firstPoint),
        b_(secondPoint),
        c_(thirdPoint) {

    vector_t vecAB(a_, b_);
    vector_t vecAC(a_, c_);

    vector_t normal = vecAB.cross(vecAC);
    if (normal.isZero()) {
        throw std::invalid_argument("Triangle points must not lie on the same line");
    }
}

double
triangle_t::lenAB() const {
    return a_.distance(b_);
}

double
triangle_t::lenBC() const {
    return b_.distance(c_);
}

double
triangle_t::lenCA() const {
    return c_.distance(a_);
}

double
vector_t::area () const {
    vector_t vecAB(a_, b_);
    vector_t vecAC(a_, c_);

    vector_t normal = vecAB.cross(vecAC);
    return normal.length() / 2;
};
    vector_t unitNormal () const;

point_t
centroid() const;

    bool intersects (const triangle_t &otherTriangle);

} // namespace geometry
