module;

#include <cmath>
#include <stdexcept>
#include <vector>
#include <string>

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
point_t::operator==(const point_t &refPoint) const {
    return isDoubleZero(refPoint.x_ - x_) &&
           isDoubleZero(refPoint.y_ - y_) &&
           isDoubleZero(refPoint.z_ - z_);
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
    if (a == b) {
        throw std::invalid_argument
            ("The line segment must be constructed using two distinct points.");
    }
}

vector_t
segment_t::direction() const {
    vector_t dir{a_, b_};
    return dir;
}

int
segment_t::sideOfLine(const point_t& p, const vector_t& planeNormal) const {
    vector_t edge = direction();
    double distance = edge.cross(vector_t{a_, p}).dot(planeNormal) / edge.length();
    if (isDoubleZero(distance)) {
        return 0;
    }
    return distance < 0.0 ? -1 : 1;
}

bool
segment_t::projectionFallsOnSegment(const point_t& p) const {
    vector_t edge = direction();
    double length = edge.length();
    double projection = vector_t{a_, p}.dot(edge) / length;
    double pastEnd = projection - length;
    return (projection >= 0.0 || isDoubleZero(projection)) &&
           (pastEnd <= 0.0 || isDoubleZero(pastEnd));
}

bool
segment_t::intersects(const segment_t& other, const vector_t& planeNormal) const {
    int sideA = other.sideOfLine(a_, planeNormal);
    int sideB = other.sideOfLine(b_, planeNormal);
    int sideC = sideOfLine(other.a_, planeNormal);
    int sideD = sideOfLine(other.b_, planeNormal);

    return (sideC * sideD < 0 && sideA * sideB < 0) ||
           (sideC == 0 && projectionFallsOnSegment(other.a_)) ||
           (sideD == 0 && projectionFallsOnSegment(other.b_)) ||
           (sideA == 0 && other.projectionFallsOnSegment(a_)) ||
           (sideB == 0 && other.projectionFallsOnSegment(b_));
}

vector_t::vector_t(const point_t &origin, const point_t &end) {
    x_ = end.x() - origin.x();
    y_ = end.y() - origin.y();
    z_ = end.z() - origin.z();
}

bool
vector_t::operator==(const vector_t &otherVector) const {
    return isDoubleZero(otherVector.x_ - x_) &&
           isDoubleZero(otherVector.y_ - y_) &&
           isDoubleZero(otherVector.z_ - z_);
};

double
vector_t::length() const {
    return std::hypot(x_, y_, z_);
}

vector_t
vector_t::normalized() const {
    double len = length();
    if (!std::isfinite(len)) {
        throw std::invalid_argument("Vector length must be finite for normalization");
    }
    if (isDoubleZero(len)) {
        throw std::invalid_argument("Cannot normalize a zero vector");
    }
    vector_t norm = (*this) * (1 / len);
    return norm;
}

vector_t
vector_t::cross(const vector_t &otherVector) const {
    return vector_t(
        y_ * otherVector.z_ - z_ * otherVector.y_,
        z_ * otherVector.x_ - x_ * otherVector.z_,
        x_ * otherVector.y_ - y_ * otherVector.x_
    );
}

double
vector_t::dot(const vector_t &otherVector) const {
    return x() * otherVector.x() + y() * otherVector.y() + z() * otherVector.z();
}

vector_t&
vector_t::operator+=(const vector_t &otherVector) {
    x_ += otherVector.x();
    y_ += otherVector.y();
    z_ += otherVector.z();

    return *this;
}

vector_t
operator+(const vector_t& x, const vector_t& y) {
    vector_t tmp{x};
    tmp += y;
    return tmp;
}

vector_t&
vector_t::operator-=(const vector_t &otherVector) {
    x_ -= otherVector.x();
    y_ -= otherVector.y();
    z_ -= otherVector.z();

    return *this;
}

vector_t
operator-(const vector_t& x, const vector_t& y) {
    vector_t tmp{x};
    tmp -= y;
    return tmp;
}

vector_t
vector_t::operator*(double number) const {
    vector_t newVec{x() * number, y() * number, z() * number};
    return newVec;
}


triangle_t::triangle_t(const point_t& firstPoint,
                       const point_t& secondPoint,
                       const point_t& thirdPoint)
    : polygon_t({firstPoint, secondPoint, thirdPoint}) {

    vector_t vecAB{point(0), point(1)};
    vector_t vecAC{point(0), point(2)};

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

void
polygon_t::validateCollinearity() const {
    std::size_t pointsNum = numOfPoints();

    for (std::size_t i = 0; i < pointsNum; ++i) {
        if (point(i) == point((i + 1) % pointsNum)) {
            throw std::invalid_argument("Adjacent polygon vertices must be distinct");
        }

        vector_t firstVec{point(i), point((i + 1) % pointsNum)};
        vector_t secondVec{point((i + 1) % pointsNum), point((i + 2) % pointsNum)};

        vector_t norm = firstVec.cross(secondVec);
        if (norm.isZero()) {
            throw std::invalid_argument("Collinear vertices: " +
                                        std::to_string(i) + ", " +
                                        std::to_string((i + 1) % pointsNum) + ", " +
                                        std::to_string((i + 2) % pointsNum));
        }
    }
}

void
polygon_t::validateCoplanarity() const {
    vector_t firstVec{point(0), point(1)};
    vector_t secondVec{point(1), point(2)};
    vector_t planeNorm = firstVec.cross(secondVec).normalized();

    for (std::size_t i = 3; i < numOfPoints(); ++i) {
        vector_t planeVector{point(0), point(i)};
        if (!isDoubleZero(planeVector.dot(planeNorm))) {
            throw std::invalid_argument(
                "Polygon vertices must lie in the same plane. Invalid vertex: " +
                std::to_string(i));
        }
    }
}

void
polygon_t::validateSelfIntersections() const {
    const vector_t normal = unitNormal();
    const std::size_t pointsNum = numOfPoints();

    for (std::size_t i = 0; i < pointsNum; ++i) {
        std::size_t nextI = (i + 1) % pointsNum;
        segment_t firstEdge{point(i), point(nextI)};
        for (std::size_t j = i + 1; j < pointsNum; ++j) {
            std::size_t nextJ = (j + 1) % pointsNum;
            if (nextI == j || nextJ == i) {
                continue;
            }

            segment_t secondEdge{point(j), point(nextJ)};
            if (firstEdge.intersects(secondEdge, normal)) {
                throw std::invalid_argument(
                    "Polygon edges intersect: " + std::to_string(i) + ", " +
                    std::to_string(j));
            }
        }
    }
}

polygon_t::polygon_t(std::vector<point_t> points)
    : points_(points) {
    if (points_.size() < 3) {
        throw std::invalid_argument("The polygon must have at least three vertices.");
    }

    validateCollinearity();
    validateCoplanarity();
    validateSelfIntersections();
}

double
polygon_t::area() const {
    vector_t crossSum {0.0, 0.0, 0.0};

    for (std::size_t i = 0; i < numOfPoints() - 2; ++i) {
        vector_t firstTriangleSide{point(0), point(i + 1)};
        vector_t secondTriangleSide{point(0), point(i + 2)};

        crossSum += firstTriangleSide.cross(secondTriangleSide);
    }

    return crossSum.length() / 2;
}

vector_t
polygon_t::unitNormal() const {
    vector_t firstVec{point(0), point(1)};
    vector_t secondVec{point(0), point(2)};

    vector_t normal = firstVec.cross(secondVec);
    return normal.normalized();
}

}
