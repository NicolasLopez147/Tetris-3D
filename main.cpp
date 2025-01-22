#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Callback para redimensionar la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Shader simple para el color de las líneas
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(0.5, 0.5, 0.5, 0.3); // Color gris semi-transparente
}
)";

// Compilar shaders y crear un programa de shaders
GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Generar datos para la grilla 3D
std::vector<float> generateGridVertices(int width, int height, int depth) {
    std::vector<float> vertices;

    // Líneas paralelas al eje Z
    for (int y = 0; y <= height; ++y) {
        for (int x = 0; x <= width; ++x) {
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(0);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(depth);
        }
    }

    // Líneas paralelas al eje X
    for (int y = 0; y <= height; ++y) {
        for (int z = 0; z <= depth; ++z) {
            vertices.push_back(0);
            vertices.push_back(y);
            vertices.push_back(z);

            vertices.push_back(width);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    // Líneas paralelas al eje Y
    for (int x = 0; x <= width; ++x) {
        for (int z = 0; z <= depth; ++z) {
            vertices.push_back(x);
            vertices.push_back(0);
            vertices.push_back(z);

            vertices.push_back(x);
            vertices.push_back(height);
            vertices.push_back(z);
        }
    }

    return vertices;
}

int main() {
    // Inicializa GLFW
    if (!glfwInit()) {
        std::cerr << "Error: No se pudo inicializar GLFW" << std::endl;
        return -1;
    }

    // Configuración de GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(800, 600, "Grilla 3D - OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error: No se pudo crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Inicializa GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error: No se pudo inicializar GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Configurar viewport y callback
    int viewportWidth, viewportHeight;
    glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);
    glViewport(0, 0, viewportWidth, viewportHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Configuración de OpenGL
    glEnable(GL_DEPTH_TEST);

    // Crear programa de shaders
    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Generar datos de la grilla
    std::vector<float> gridVertices = generateGridVertices(10, 20, 10);
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Matrices de proyección y vista
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewportWidth / viewportHeight, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(15, 25, 15), glm::vec3(5, 10, 5), glm::vec3(0, 1, 0));

    // Ciclo principal
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");

        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
