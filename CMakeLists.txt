cmake_minimum_required(VERSION 3.24)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not supported. Use a separate build directory.")
endif()

project(Triangles VERSION 0.1.0 LANGUAGES CXX)

# The geometry and renderer headers are interface sketches for now.
option(TRIANGLES_BUILD_APP "Build the current main.cpp placeholder" OFF)

function(triangles_configure_target target)
    target_compile_features(${target} PRIVATE cxx_std_17)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

if(TRIANGLES_BUILD_APP)

    set(GENERAL_FUNCTIONS_DIR "$ENV{GENERAL_FUNCTIONS_DIR}"
    CACHE PATH "Path to generalFunctions sources")

    if(NOT EXISTS "${GENERAL_FUNCTIONS_DIR}/file.cpp")
        message(FATAL_ERROR
            "generalFunctions not found. Run inside nix develop "
            "or pass -DGENERAL_FUNCTIONS_DIR=/path/to/generalFunctions")
    endif()

    add_library(general_functions STATIC
        "${GENERAL_FUNCTIONS_DIR}/file.cpp"
        "${GENERAL_FUNCTIONS_DIR}/lexer/lexer.cpp"
        "${GENERAL_FUNCTIONS_DIR}/lexer/token_stream.cpp"
    )

    target_include_directories(general_functions PUBLIC
        "${GENERAL_FUNCTIONS_DIR}"
    )

    target_compile_features(general_functions PUBLIC cxx_std_17)

    add_executable(triangles
        main.cpp
        parser/parser.cpp
    )

    target_link_libraries(triangles PRIVATE general_functions)

    target_include_directories(triangles PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/polygonProcessor"
    )

    triangles_configure_target(triangles)
endif()
