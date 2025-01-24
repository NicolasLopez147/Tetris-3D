#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "src/Game.cpp"

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
    srand(static_cast<unsigned>(time(nullptr))); // Seed the random number generator
// Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "Error initializing GLFW" << std::endl;
            return 0;
        }
    
        // Create a window
        GLFWwindow* window = glfwCreateWindow(1600, 1200, "Tetromino Demo", nullptr, nullptr);
        if (!window) {
            std::cerr << "Error creating window" << std::endl;
            glfwTerminate();
            return 0;
        }
 
        glfwMakeContextCurrent(window);
        glewExperimental = true;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Error initializing GLEW" << std::endl;
            return 0;
        }
 
        glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set background color
 
        // Vector3D pos(1.0f, 0.0f, 0.0f); // Initial position for the Tetromino
 
        // Tetromino tetrominoI(pos,2); // Create a Tetromino of type I-shape
 
        bool wPressed = false;
        bool qPressed = false;
        bool ePressed = false;
 
        Game game = Game(10,20,10);
        game.start();
 
        while (!glfwWindowShouldClose(window) && game.getIsRunning()) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen
 
            // Draw the Tetromino
            game.update(0.0f);
 
            glfwSwapBuffers(window); // Swap front and back buffers
            glfwPollEvents(); // Poll for and process events
        }
 
        glfwTerminate(); // Clean up and terminate GLFW
        return 0;

}
