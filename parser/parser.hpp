#pragma once

#include <filesystem>

#include "initPolysData.hpp"

namespace lexer {
class TokenStream;
}

namespace triangles {

class Parser {
public:
    InitPolysData parse(const std::filesystem::path& path) const;
    InitPolysData parse(lexer::TokenStream& tokens) const;
};

} // namespace triangles
