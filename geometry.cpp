module;

#include <cmath>
#include <vector.h>

export module geometry;

namespace geometry {

class point_t {

private:
    double x_ = NAN;
    double y_ = NAN;
    double z_ = NAN;

public:
    point_t(double x, double y, double z);

    bool equal(const point_t &refPoint) const;
    double distance(const point_t &otherPoint) const;
};

class vector_t {
    double x_ = NAN;
    double y_ = NAN;
    double z_ = NAN;

public:
    vector_t(double x, double y, double z) : x_(x), y_(y), z_(z) {};
    vector_t(const point_t &a, const point_t &b);

    double length() const;
    bool isZero() const { return (this->length < EPSILON); }

    bool equal(const vector_t &refVector) const;
    vector_t cross (const vector_t &otherVector) const;
}

class triangle_t {
private:
    point_t a_;
    point_t b_;
    point_t c_;
public:
    triangle_t(const point_t &firstPoint,
               const point_t &secondPoint,
               const point_t &thirdPoint);

    const point_t& a() const { return a_ };
    const point_t& b() const { return b_ };
    const point_t& c() const { return c_ };

    double lenAB() const;
    double lenBC() const;
    double lenCA() const;

    double area () const;
    vector_t unitNormal () const;
    point_t centroid() const;

    bool intersects (const triangle_t &otherTriangle);
}

} // namespace geometry
