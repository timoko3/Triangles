#include <filesystem>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "lexer/lexer.h"
#include "parser/parser.hpp"

namespace {

triangles::InitPolysData parse(const std::string& source) {
    auto tokens = lexer::Lexer(source, "scene.txt").tokenize();
    return triangles::Parser{}.parse(tokens);
}

TEST(Parser, ReadsIntegerAndFractionalCoordinates) {
    const auto result = parse("1\n0 -2 3\n0.5 -1.25 +2.5\n4 5 6");
    ASSERT_EQ(result.data.size(), 1U);
    ASSERT_EQ(result.data[0].size(), 3U);
    EXPECT_DOUBLE_EQ(result.data[0][0].x(), 0);
    EXPECT_DOUBLE_EQ(result.data[0][0].y(), -2);
    EXPECT_DOUBLE_EQ(result.data[0][0].z(), 3);
    EXPECT_DOUBLE_EQ(result.data[0][1].x(), 0.5);
    EXPECT_DOUBLE_EQ(result.data[0][1].y(), -1.25);
    EXPECT_DOUBLE_EQ(result.data[0][1].z(), 2.5);
    EXPECT_DOUBLE_EQ(result.data[0][2].x(), 4);
    EXPECT_DOUBLE_EQ(result.data[0][2].y(), 5);
    EXPECT_DOUBLE_EQ(result.data[0][2].z(), 6);
}

TEST(Parser, ReadsMultipleTrianglesAndConsumesInput) {
    auto tokens =
        lexer::Lexer("2 0 0 0 1 0 0 0 1 0\n\n2 3 4 5 6 7 8 9 10 \t\n").tokenize();
    const auto result = triangles::Parser{}.parse(tokens);
    ASSERT_EQ(result.data.size(), 2U);
    ASSERT_EQ(result.data[1].size(), 3U);
    EXPECT_DOUBLE_EQ(result.data[1][0].x(), 2);
    EXPECT_DOUBLE_EQ(result.data[1][2].z(), 10);
    EXPECT_TRUE(tokens.atEnd());
}

class InvalidParserInput : public testing::TestWithParam<const char*> {};

TEST_P(InvalidParserInput, RejectsInvalidScene) {
    EXPECT_THROW(parse(GetParam()), std::runtime_error);
}

INSTANTIATE_TEST_SUITE_P(MalformedScenes, InvalidParserInput,
                         testing::Values("", "0", "-1", "1000000", "1000001", "1.5",
                                         "name", "1", "1 0 0 0 1 0 0 0 1",
                                         "2 0 0 0 1 0 0 0 1 0",
                                         "1 name 0 0 1 0 0 0 1 0",
                                         "1 0 0 0 1 0 0 0 1 0 42"));

TEST(Parser, ReportsFilePositionAndVertexForMissingCoordinate) {
    try {
        (void)parse("1\n0 0 0\n1 0 0\n0 1");
        FAIL() << "Expected missing z coordinate to fail";
    } catch (const std::runtime_error& error) {
        const std::string message = error.what();
        EXPECT_NE(message.find("scene.txt:4:4:"), std::string::npos);
        EXPECT_NE(message.find("Expected z coordinate (triangle 1, vertex 3)"),
                  std::string::npos);
    }
}

TEST(Parser, ReadsSceneFromFile) {
    const auto result = triangles::Parser{}.parse(
        std::filesystem::path(TRIANGLES_SOURCE_DIR) / "inputs/input_1.txt");
    ASSERT_EQ(result.data.size(), 1U);
    ASSERT_EQ(result.data[0].size(), 3U);
    EXPECT_DOUBLE_EQ(result.data[0][1].x(), 2);
    EXPECT_DOUBLE_EQ(result.data[0][2].y(), 2);
}

TEST(Parser, RejectsMissingFile) {
    EXPECT_THROW(
        triangles::Parser{}.parse(std::filesystem::path(TRIANGLES_SOURCE_DIR) /
                                  "tests/nonexistent-directory/scene.txt"),
        std::runtime_error);
}

} // namespace
