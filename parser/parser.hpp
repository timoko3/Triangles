#pragma once

#include <filesystem>

#include "initPolysData.hpp"

namespace lexer { class TokenStream; }

namespace HW3D {

class Parser {
public:
    InitPolysData parse(const std::filesystem::path& path) const;
    InitPolysData parse(lexer::TokenStream& tokens) const;
};

} // namespace HW3D
