#include <iostream>

import geometry;

int main() {
    const geometry::triangle_t triangle{{0, 0, 0}, {3, 0, 0}, {0, 4, 0}};

    if (triangle.area() != 6.0 || triangle.lenBC() != 5.0) {
        std::cerr << "Unexpected geometry result for a 3-4-5 triangle\n";
        return 1;
    }

    return 0;
}
