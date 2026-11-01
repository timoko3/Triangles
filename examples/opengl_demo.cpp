#include <iostream>
#include <algorithm>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const float vertices[] = {
    // Положение         Цвет RGB          Нормаль
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
};

const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;

uniform mat4 mvp;

out vec3 vertexColor;
out vec3 surfaceNormal;
out vec3 worldPosition;

void main() {
    gl_Position = mvp * vec4(position, 1.0);
    vertexColor = color;
    surfaceNormal = normal;
    worldPosition = position;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

in vec3 vertexColor;
in vec3 surfaceNormal;
in vec3 worldPosition;

uniform vec3 towardLight;
uniform vec3 cameraPosition;

out vec4 fragmentColor;

void main() {
    vec3 n = normalize(surfaceNormal);
    vec3 l = normalize(towardLight);
    vec3 v = normalize(cameraPosition - worldPosition);

    float diffuse = max(dot(n, l), 0.0);

    vec3 reflected = reflect(-l, n);

    float specular = 0.0;
    if (diffuse > 0.0) {
        specular = pow(max(dot(v, reflected), 0.0), 32.0);
    }

    vec3 base = vertexColor * (0.15 + 0.65 * diffuse);
    vec3 highlight = vec3(0.5) * specular;

    fragmentColor = vec4(base + highlight, 1.0);
}
)";

GLuint compileShader(GLenum type, const char* source) {
    const GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE) {
        char log[2048] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compilation failed:\n" << log << '\n';

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int main() {
    glfwSetErrorCallback([](int code, const char* description) {
        std::cerr << "GLFW error " << code << ": "
              << description << '\n';
    });

    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    std::cout << "GLFW initialized\n";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangles demo", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (gladLoadGL(glfwGetProcAddress) == 0) {
        std::cerr << "Failed to load OpenGL\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glfwSwapInterval(1);


    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(
        window,
        [](GLFWwindow*, int newWidth, int newHeight) {
            glViewport(0, 0, newWidth, newHeight);
        }
    );

    GLuint vao = 0;
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        9 * sizeof(float),
        nullptr
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        9 * sizeof(float),
        reinterpret_cast<const void*>(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2, 3, GL_FLOAT, GL_FALSE,
        9 * sizeof(float),
        reinterpret_cast<const void*>(6 * sizeof(float))
    );
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);



    const GLuint vertexShader   = compileShader(GL_VERTEX_SHADER, vertexShaderSource);

    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    if (vertexShader == 0 || fragmentShader == 0) {
        if (vertexShader != 0) {
            glDeleteShader(vertexShader);
        }
        if (fragmentShader != 0) {
            glDeleteShader(fragmentShader);
        }

        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    const GLint mvpLocation = glGetUniformLocation(program, "mvp");

    const GLint lightLocation  = glGetUniformLocation(program, "towardLight");
    const GLint cameraLocation = glGetUniformLocation(program, "cameraPosition");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (linked != GL_TRUE) {
        char log[2048] = {};
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Shader program linking failed:\n" << log << '\n';

        glDeleteProgram(program);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    float yaw = 0.0f;
    float pitch = 0.0f;
    float radius = 2.0f;

    const float angularSpeed = glm::radians(60.0f);
    const float zoomSpeed = 1.5f;

    double previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        const double currentTime = glfwGetTime();
        const float deltaTime = std::min(
            static_cast<float>(currentTime - previousTime),
            0.1f
        );
        previousTime = currentTime;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            yaw -= angularSpeed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            yaw += angularSpeed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            pitch += angularSpeed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            pitch -= angularSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            radius -= zoomSpeed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            radius += zoomSpeed * deltaTime;
        }

        pitch = std::clamp(
            pitch,
            glm::radians(-85.0f),
            glm::radians(85.0f)
        );
        radius = std::clamp(radius, 1.0f, 10.0f);

        glfwGetFramebufferSize(window, &width, &height);

        if (width <= 0 || height <= 0) {
            glfwWaitEvents();
            continue;
        }

        const glm::vec3 target(0.0f, 0.0f, 0.0f);

        const glm::vec3 offset(
            radius * glm::cos(pitch) * glm::sin(yaw),
            radius * glm::sin(pitch),
            radius * glm::cos(pitch) * glm::cos(yaw)
        );

        const glm::vec3 cameraPosition = target + offset;

        const glm::mat4 model(1.0f);

        const glm::mat4 view = glm::lookAt(
            cameraPosition,
            target,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );


        const glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f,
            100.0f
        );

        const glm::mat4 mvp = projection * view * model;

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniform3fv(
            cameraLocation,
            1,
            glm::value_ptr(cameraPosition)
        );

        glUniformMatrix4fv(
            mvpLocation,
            1,
            GL_FALSE,
            glm::value_ptr(mvp)
        );

        glUniform3f(lightLocation, 0.0f, 0.0f, 1.0f);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glUseProgram(0);
    glDeleteProgram(program);

    glfwDestroyWindow(window);

    glfwTerminate();
}
