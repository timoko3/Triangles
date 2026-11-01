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

    set(GENERAL_FUNCTIONS_DIR ""
        CACHE PATH "Override path to generalFunctions sources")
    if(GENERAL_FUNCTIONS_DIR)
        set(general_functions_dir "${GENERAL_FUNCTIONS_DIR}")
    else()
        set(general_functions_dir "$ENV{GENERAL_FUNCTIONS_DIR}")
    endif()

    if(NOT EXISTS "${general_functions_dir}/file.cpp")
        message(FATAL_ERROR
            "generalFunctions not found at '${general_functions_dir}'. Run inside nix develop "
            "or pass -DGENERAL_FUNCTIONS_DIR=/path/to/generalFunctions")
    endif()

    add_library(general_functions STATIC
        "${general_functions_dir}/file.cpp"
        "${general_functions_dir}/lexer/lexer.cpp"
        "${general_functions_dir}/lexer/token_stream.cpp"
    )

    target_include_directories(general_functions PUBLIC
        "${general_functions_dir}"
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

option(TRIANGLES_BUILD_OPENGL_DEMO "Build the OpenGL example" OFF)

if(TRIANGLES_BUILD_OPENGL_DEMO)
    enable_language(C)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)

    set(GLAD_SOURCE_DIR ""
        CACHE PATH "Override path to generated GLAD 2 sources")
    if(GLAD_SOURCE_DIR)
        set(glad_source_dir "${GLAD_SOURCE_DIR}")
    else()
        set(glad_source_dir "$ENV{GLAD_SOURCE_DIR}")
    endif()

    if(NOT EXISTS "${glad_source_dir}/src/gl.c" OR
       NOT EXISTS "${glad_source_dir}/include/glad/gl.h")
        message(FATAL_ERROR
            "GLAD 2 sources not found at '${glad_source_dir}'. Run inside nix develop "
            "or pass -DGLAD_SOURCE_DIR=/path/to/generated/glad")
    endif()

    add_library(glad STATIC "${glad_source_dir}/src/gl.c")
    target_compile_features(glad PRIVATE c_std_99)
    target_include_directories(glad SYSTEM PUBLIC "${glad_source_dir}/include")

    add_executable(opengl_demo examples/opengl_demo.cpp)
    target_link_libraries(opengl_demo PRIVATE glfw glad glm::glm)
    target_compile_definitions(opengl_demo PRIVATE GLFW_INCLUDE_NONE)

    triangles_configure_target(opengl_demo)
endif()
