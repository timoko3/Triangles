#include <exception>
#include <filesystem>
#include <iostream>

#include "parser/parser.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: triangles_input <scene-file>\n";
        return 1;
    }

    try {
        const auto data = triangles::Parser{}.parse(std::filesystem::path{argv[1]});
        std::cout << "Read " << data.data.size() << " triangles\n";
        for (std::size_t i = 0; i < data.data.size(); ++i) {
            std::cout << "Triangle " << i + 1 << ":\n";
            for (const auto& point : data.data[i]) {
                std::cout << "  " << point.x() << ' ' << point.y() << ' '
                          << point.z() << '\n';
            }
        }

    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
