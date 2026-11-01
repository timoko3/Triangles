#include <gtest/gtest.h>

import geometry;

TEST(GeometryIntegration, RightTriangleAreaAndHypotenuse) {
    const geometry::triangle_t triangle{{0, 0, 0}, {3, 0, 0}, {0, 4, 0}};
    EXPECT_DOUBLE_EQ(triangle.area(), 6.0);
    EXPECT_DOUBLE_EQ(triangle.lenBC(), 5.0);
}
