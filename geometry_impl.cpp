module;

#include <cmath>
#include <stdexcept>
#include <vector>

module geometry;

namespace geometry {

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
    double dx = x_ - otherPoint.x();
    double dy = y_ - otherPoint.y();
    double dz = z_ - otherPoint.z();

    return std::hypot(dx, dy, dz);
};

segment_t::segment_t(const point_t &a, const point_t &b)
    : a_(a),
      b_(b) {
    if (a.equal(b)) {
        throw std::invalid_argument
            ("The line segment must be constructed using two distinct points.");
    }
}

vector_t
segment_t::direction() const {
    vector_t dir(a_, b_);
    return dir;
}

vector_t::vector_t(const point_t &origin, const point_t &end) {
    x_ = end.x() - origin.x();
    y_ = end.y() - origin.y();
    x_ = end.z() - origin.z();
}

bool
vector_t::equal(const vector_t &refVector) const {
    return ((std::fabs(refVector.x_ - x_) < EPSILON)&&
            (std::fabs(refVector.y_ - y_) < EPSILON)&&
            (std::fabs(refVector.z_ - z_) < EPSILON));
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

vector_t
vector_t::mulByNum(double number) {
    vector_t newVec(x() * number, y() * number, z() * number);
    return newVec;
}


triangle_t::triangle_t(const point_t& firstPoint,
                       const point_t& secondPoint,
                       const point_t& thirdPoint)
    : polygon_t({firstPoint, secondPoint, thirdPoint}) {

    vector_t vecAB(point(0), point(1));
    vector_t vecAC(point(0), point(2));

    vector_t normal = vecAB.cross(vecAC);
    if (normal.isZero()) {
        throw std::invalid_argument("Triangle points must not lie on the same line");
    }
}

double
triangle_t::lenAB() const {
    return point(0).distance(point(1));
}

double
triangle_t::lenBC() const {
    return point(1).distance(point(2));
}

double
triangle_t::lenCA() const {
    return point(2).distance(point(0));
}

polygon_t::polygon_t(std::vector<point_t> points)
    : points_(points) {
    if (points.size() < 3) {
        throw std::invalid_argument("The polygon must have at least three vertices.");
    }
}

double
polygon_t::area() const {
    double area = 0;

    for (std::size_t i = 0; i < size() - 2; i++) {
        vector_t firstTriangleSide(point(0), point(i + 1));
        vector_t secondTriangleSide(point(0), point(i + 2));

        vector_t crossProd = firstTriangleSide.cross(secondTriangleSide);
        area += crossProd.length() / 2;
    }

    return area;
}

vector_t
polygon_t::unitNormal() const {
    vector_t firstVec(point(0), point(1));
    vector_t secondVec(point(0), point(2));

    vector_t normal = firstVec.cross(secondVec);
    return normal.mulByNum(1 / normal.length());
}

} // namespace geometry
