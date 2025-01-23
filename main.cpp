#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Callback function to adjust the OpenGL viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Vertex shader: Transforms vertex positions and passes fragment position to the fragment shader
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;  // Projection matrix
uniform mat4 view;        // View (camera) matrix

out vec3 FragPos; // Pass the position to the fragment shader

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);  // Transform to clip space
    FragPos = aPos;  // Pass the position for further calculations
}
)";

// Fragment shader: Colors the grid lines with transparency based on their height
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;  // Output color of the fragment

in vec3 FragPos;  // Position of the fragment

void main() {
    float alpha = 1.0 - abs(FragPos.y) / 20.0;  // Calculate transparency based on Y-axis height
    FragColor = vec4(0.5, 0.5, 0.5, alpha);    // Grey color with varying transparency
}
)";

// Compile and link the vertex and fragment shaders into a shader program
GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    // Compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);

    // Compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);

    // Link the shaders into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Clean up the individual shaders as they are no longer needed
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Generate vertices for a 3D grid based on width, height, and depth
std::vector<float> generateGridVertices(int width, int height, int depth) {
    std::vector<float> vertices;

    // Horizontal lines (parallel to X-axis) at every Y-level
    for (int y = 0; y <= height; ++y) {
        vertices.push_back(0);  // Start of line
        vertices.push_back(y);
        vertices.push_back(0);

        vertices.push_back(width);  // End of line
        vertices.push_back(y);
        vertices.push_back(0);

        vertices.push_back(0);  // Start of depth line
        vertices.push_back(y);
        vertices.push_back(0);

        vertices.push_back(0);
        vertices.push_back(y);
        vertices.push_back(depth);
    }

    // Vertical lines (parallel to Z-axis) from floor to ceiling
    for (int z = 0; z <= depth; ++z) {
        vertices.push_back(0);  // Start of vertical line
        vertices.push_back(0);
        vertices.push_back(z);

        vertices.push_back(0);  // End of vertical line
        vertices.push_back(height);
        vertices.push_back(z);

        vertices.push_back(0);  // Horizontal lines at base level
        vertices.push_back(0);
        vertices.push_back(z);

        vertices.push_back(width);
        vertices.push_back(0);
        vertices.push_back(z);
    }

    // Vertical lines (parallel to Y-axis) at the front face
    for (int x = 0; x <= width; ++x) {
        vertices.push_back(x);  // Start of vertical line
        vertices.push_back(0);
        vertices.push_back(0);

        vertices.push_back(x);  // End of vertical line
        vertices.push_back(height);
        vertices.push_back(0);

        vertices.push_back(x);  // Horizontal lines at base level
        vertices.push_back(0);
        vertices.push_back(0);

        vertices.push_back(x);
        vertices.push_back(0);
        vertices.push_back(depth);
    }

    return vertices;
}

// Generate vertices for a cube with a given size and position
std::vector<float> generateCubeVertices(float x, float y, float z, float size) {
    return {
        // Front face
        x, y, z,  x + size, y, z,  x + size, y + size, z,  x, y + size, z,
        // Back face
        x, y, z - size,  x + size, y, z - size,  x + size, y + size, z - size,  x, y + size, z - size,
        // Left face
        x, y, z,  x, y, z - size,  x, y + size, z - size,  x, y + size, z,
        // Right face
        x + size, y, z,  x + size, y, z - size,  x + size, y + size, z - size,  x + size, y + size, z,
        // Top face
        x, y + size, z,  x + size, y + size, z,  x + size, y + size, z - size,  x, y + size, z - size,
        // Bottom face
        x, y, z,  x + size, y, z,  x + size, y, z - size,  x, y, z - size
    };
}

// Process keyboard inputs to move the cube
void processInput(GLFWwindow* window, glm::vec3& cubePosition) {
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cubePosition.x -= 0.1f;  // Move left
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cubePosition.x += 0.1f;  // Move right
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cubePosition.y += 0.1f;  // Move up
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cubePosition.y -= 0.1f;  // Move down
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Error: Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1600, 1200, "3D Grid - OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error: Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Configure OpenGL viewport and settings
    int viewportWidth, viewportHeight;
    glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);
    glViewport(0, 0, viewportWidth, viewportHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glEnable(GL_DEPTH_TEST);  // Enable depth testing for 3D rendering
    glEnable(GL_BLEND);       // Enable blending for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create the shader program
    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Generate grid vertices and set up a VAO/VBO for the grid
    std::vector<float> gridVertices = generateGridVertices(6, 15, 6);
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

    // Generate cube vertices and set up a VAO/VBO for the cube
    glm::vec3 cubePosition(0.0f, 0.0f, 1.0f);
    float cubeSize = 1.0f;
    std::vector<float> cubeVertices = generateCubeVertices(cubePosition.x, cubePosition.y, cubePosition.z, cubeSize);
    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), cubeVertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Set up projection and view matrices
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewportWidth / viewportHeight, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(15, 25, 15), glm::vec3(5, 10, 5), glm::vec3(0, 1, 0));

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window, cubePosition);  // Update cube position based on user input

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear buffers

        glUseProgram(shaderProgram);  // Activate shader program

        // Update model matrix for the cube
        glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePosition);

        // Pass matrices to the shader
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Render the grid
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);
        glBindVertexArray(0);

        // Render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size() / 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);  // Swap buffers
        glfwPollEvents();        // Process events
    }

    // Clean up resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
