#include "parser.hpp"

#include <string>
#include <utility>

#include "file.h"
#include "lexer/lexer.h"
#include "lexer/token_stream.h"

namespace triangles {

constexpr int MAX_NUM_OF_POLY = 1'000'000;

InitPolysData Parser::parse(const std::filesystem::path& path) const {
    lexer::Lexer input(generalFunctions::readFile(path), path);
    auto tokens = input.tokenize();
    return parse(tokens);
}

InitPolysData Parser::parse(lexer::TokenStream& tokens) const {
    const lexer::Token& countToken = tokens.peekToken();
    const auto count = tokens.expectInt("Expected an integer triangle count N");

    if (count <= 0 || count >= MAX_NUM_OF_POLY) {
        tokens.failAtToken(countToken, "Triangle count must be 0 < N < 1000000");
    }

    InitPolysData result;
    result.data.reserve(static_cast<std::size_t>(count));

    for (int triangle = 0; triangle < count; ++triangle) {
        std::vector<Point> vertices;
        vertices.reserve(3);

        for (std::size_t vertex = 0; vertex < 3; ++vertex) {
            const std::string context = " (triangle " +
                                        std::to_string(triangle + 1) + ", vertex " +
                                        std::to_string(vertex + 1) + ")";

            const double x = tokens.expectDouble("Expected x coordinate" + context);
            const double y = tokens.expectDouble("Expected y coordinate" + context);
            const double z = tokens.expectDouble("Expected z coordinate" + context);

            vertices.emplace_back(x, y, z);
        }
        result.data.push_back(std::move(vertices));
    }

    tokens.expectToken(lexer::TokenType::END, "Extra data after the last triangle");
    return result;
}

} // namespace triangles
