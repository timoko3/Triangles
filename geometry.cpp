module;

#include <cmath>
#include <vector>

export module geometry;

namespace geometry {

constexpr double EPSILON = 1e-9;

class point_t {
private:
    double x_ = NAN;
    double y_ = NAN;
    double z_ = NAN;

public:
    point_t(double x, double y, double z);

    double x() const { return x_; };
    double y() const { return y_; };
    double z() const { return z_; };

    bool equal(const point_t &refPoint) const;
    double distance(const point_t &otherPoint) const;
};

class vector_t {
    double x_ = NAN;
    double y_ = NAN;
    double z_ = NAN;

public:
    vector_t(double x, double y, double z) : x_(x), y_(y), z_(z) {};
    vector_t(const point_t &origin, const point_t &end);

    double x() const { return x_; };
    double y() const { return y_; };
    double z() const { return z_; };

    double length() const;
    bool isZero() const { return (length() < EPSILON); }

    bool equal(const vector_t &refVector) const;
    vector_t cross(const vector_t &otherVector) const;
    vector_t mulByNum(double number);
};


class segment_t {

private:
    point_t a_;
    point_t b_;

public:
    segment_t(const point_t &a, const point_t &b);

    const point_t &a() const { return a_; };
    const point_t &b() const { return b_; };

    double length() const { return a_.distance(b_); };
    vector_t direction() const;
};

class polygon_t {
private:
    std::vector<point_t> points_;

public:
    polygon_t(std::vector<point_t> points);

    std::size_t size() const { return points_.size(); };
    const std::vector<point_t>& points() const { return points_; };
    const point_t& point(std::size_t i) const { return points()[i]; };

    double area() const;
    vector_t unitNormal() const;
};

class triangle_t : public polygon_t {
public:
    triangle_t(const point_t &firstPoint,
               const point_t &secondPoint,
               const point_t &thirdPoint);

    const point_t& a() const { return point(0); };
    const point_t& b() const { return point(1); };
    const point_t& c() const { return point(2); };

    double lenAB() const;
    double lenBC() const;
    double lenCA() const;
};

} // namespace geometry
