cmake_minimum_required(VERSION 3.28)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not supported. Use a separate build directory.")
endif()

project(Triangles VERSION 0.1.0 LANGUAGES CXX)

option(TRIANGLES_BUILD_APP "Build the triangle input parser" OFF)

function(triangles_configure_target target)
    target_compile_features(${target} PRIVATE cxx_std_20)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

if(TRIANGLES_BUILD_APP)
    set(GENERAL_FUNCTIONS_SOURCE_DIR "" CACHE PATH
        "Optional local generalFunctions checkout; otherwise use the installed package")

    if(GENERAL_FUNCTIONS_SOURCE_DIR)
        if(NOT EXISTS "${GENERAL_FUNCTIONS_SOURCE_DIR}/CMakeLists.txt")
            message(FATAL_ERROR
                "GENERAL_FUNCTIONS_SOURCE_DIR must contain the generalFunctions CMake project")
        endif()

        set(GENERAL_FUNCTIONS_BUILD_LEXER ON)
        set(GENERAL_FUNCTIONS_BUILD_FILES ON)
        set(GENERAL_FUNCTIONS_BUILD_GEOMETRY ON)
        set(GENERAL_FUNCTIONS_BUILD_TESTS OFF)
        add_subdirectory("${GENERAL_FUNCTIONS_SOURCE_DIR}"
            "${CMAKE_CURRENT_BINARY_DIR}/general-functions" EXCLUDE_FROM_ALL)
    else()
        find_package(GeneralFunctions CONFIG REQUIRED COMPONENTS lexer files geometry)
    endif()

    add_executable(triangles
        main.cpp
        parser/parser.cpp
    )

    target_link_libraries(triangles PRIVATE
        GeneralFunctions::lexer
        GeneralFunctions::files
        GeneralFunctions::geometry)

    set_property(TARGET triangles PROPERTY CXX_SCAN_FOR_MODULES ON)

    target_include_directories(triangles PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/polygonProcessor"
    )

    triangles_configure_target(triangles)

    include(CTest)
    if(BUILD_TESTING)
        add_executable(geometry_smoke tests/geometry_smoke.cpp)
        target_link_libraries(geometry_smoke PRIVATE GeneralFunctions::geometry)
        triangles_configure_target(geometry_smoke)
        set_property(TARGET geometry_smoke PROPERTY CXX_SCAN_FOR_MODULES ON)
        add_test(NAME geometry_smoke COMMAND geometry_smoke)
    endif()
endif()

option(TRIANGLES_BUILD_OPENGL_DEMO "Build the OpenGL example" OFF)
option(TRIANGLES_BUILD_VISUALIZER "Build the visualizer window and backend factory" OFF)

if(TRIANGLES_BUILD_OPENGL_DEMO OR TRIANGLES_BUILD_VISUALIZER)
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

endif()

if(TRIANGLES_BUILD_VISUALIZER)
    find_package(Threads REQUIRED)
    add_library(triangles_visualizer
        visualizer/backendFactory.cpp
        visualizer/scene/camera.cpp
        visualizer/scene/scene.cpp
        visualizer/window/glfwWindow.cpp
        visualizer/visualizer.cpp
        visualizer/renderer/openGLRenderer.cpp
        visualizer/renderer/openGLRenderDataPreparer.cpp
        visualizer/renderer/matrices.cpp
        visualizer/renderer/shaderProgram.cpp)
    target_include_directories(triangles_visualizer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(triangles_visualizer PRIVATE glfw glad glm::glm Threads::Threads)
    target_compile_definitions(triangles_visualizer PRIVATE GLFW_INCLUDE_NONE)
    target_compile_features(triangles_visualizer PUBLIC cxx_std_20)
    triangles_configure_target(triangles_visualizer)
    set_property(TARGET triangles_visualizer PROPERTY CXX_SCAN_FOR_MODULES OFF)

    add_executable(renderTest renderTest.cpp)
    target_link_libraries(renderTest PRIVATE triangles_visualizer)
    triangles_configure_target(renderTest)
    set_property(TARGET renderTest PROPERTY CXX_SCAN_FOR_MODULES OFF)
endif()

if(TRIANGLES_BUILD_OPENGL_DEMO)
    add_executable(opengl_demo examples/opengl_demo.cpp)
    target_link_libraries(opengl_demo PRIVATE glfw glad glm::glm)
    target_compile_definitions(opengl_demo PRIVATE GLFW_INCLUDE_NONE)

    triangles_configure_target(opengl_demo)
endif()
