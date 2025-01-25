#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include "src/Game.cpp"

// Callback function to adjust the OpenGL viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Vertex shader: Transforms vertex positions and passes fragment position to the fragment shader
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;


uniform mat4 model;      // Model matrix
uniform mat4 projection; // Projection matrix
uniform mat4 view;       // View (camera) matrix

out vec3 FragPos; // Pass the position to the fragment shader
out float FragHeight; // Pass the height to the fragment shader

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);  // Transform to clip space
    FragPos = vec3(model * vec4(aPos, 1.0));                   // Transform the position
    FragHeight = aPos.y;                                       // Set the height
}
)";

// Fragment shader: Colors the grid lines with transparency based on their height
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;  // Output color of the fragment

uniform vec3 cubeColor; // Color of the cube
uniform bool isGRID; // Color of the cube

in vec3 FragPos;  // Position of the fragment
in float FragHeight; // Height of the fragment

void main() {
    if(isGRID){
        float alpha = 1.0 - clamp(abs(FragHeight) / 18.0, 0.0, 1.0);  // Calculate transparency based on height
        FragColor = vec4(1.0, 1.0, 1.0, alpha);  // Set the color of the fragment  
    }else{
        FragColor = vec4(cubeColor,1.0);  // Set the color of the fragment
    }
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

    for (int y = 0; y <= height; ++y) {
        // Líneas horizontales
        vertices.insert(vertices.end(), { 0, (float)y, 0, (float)width, (float)y, 0 });
        vertices.insert(vertices.end(), { 0, (float)y, 0, 0, (float)y, (float)depth });
    }

    for (int z = 0; z <= depth; ++z) {
        // Líneas verticales
        vertices.insert(vertices.end(), { 0, 0, (float)z, 0, (float)height, (float)z });
        vertices.insert(vertices.end(), { 0, 0, (float)z, (float)width, 0, (float)z });
    }

    for (int x = 0; x <= width; ++x) {
        vertices.insert(vertices.end(), { (float)x, 0, 0, (float)x, (float)height, 0 });
        vertices.insert(vertices.end(), { (float)x, 0, 0, (float)x, 0, (float)depth });
    }

    return vertices;
}


std::vector<float> cubeVertices = {
    // Posiciones de los vértices
    0.0f, 0.0f, 0.0f,  // 0: Bottom-left-back
     1.0f, 0.0f, 0.0f,  // 1: Bottom-right-back
     1.0f,  1.0f, 0.0f,  // 2: Top-right-back
    0.0f,  1.0f, 0.0f,  // 3: Top-left-back
    0.0f, 0.0f,  1.0f,  // 4: Bottom-left-front
     1.0f, 0.0f,  1.0f,  // 5: Bottom-right-front
     1.0f,  1.0f,  1.0f,  // 6: Top-right-front
    0.0f,  1.0f,  1.0f   // 7: Top-left-front
};

std::vector<unsigned int> cubeIndices = {
    // Back face
    0, 1, 2,
    2, 3, 0,
    // Front face
    4, 5, 6,
    6, 7, 4,
    // Left face
    0, 4, 7,
    7, 3, 0,
    // Right face
    1, 5, 6,
    6, 2, 1,
    // Bottom face
    0, 1, 5,
    5, 4, 0,
    // Top face
    3, 2, 6,
    6, 7, 3
};


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

GLuint createVAO(const std::vector<float>& vertices, int vertexSize) {
    GLuint VAO, VBO , EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int), cubeIndices.data(), GL_STATIC_DRAW);


    glVertexAttribPointer(0, vertexSize, GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
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
    GLuint gridVAO = createVAO(gridVertices, 3);


    // Generate cube vertices and set up a VAO/VBO for the cube
    glm::vec3 cube1Position(0.0f, 0.0f, 0.0f);
    glm::vec3 cube2Position(1.0f, 0.0f, 0.0f);

    glm::vec3 targetOffset(0.0f,0.0f,0.0f);
    float cubeSize = 1.0f;
    // std::vector<float> cubeVertices = generateCubeVertices(0.0f,0.0f,1.0f, cubeSize);
    GLuint cubeVAO = createVAO(cubeVertices, 3);

    // Set up projection and view matrices
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewportWidth / viewportHeight, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(15, 25, 15), glm::vec3(5, 10, 5), glm::vec3(0, 1, 0));

    // Render loop
    while (!glfwWindowShouldClose(window)) {

        // Calcular las nuevas posiciones interpoladas
        glm::vec3 cube1NewPosition = cube1Position + targetOffset;
        glm::vec3 cube2NewPosition = cube2Position + targetOffset;

        // Limpiar la pantalla
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glUseProgram(shaderProgram);

        // Enviar matrices de proyección y vista al shader
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        
        // Dibujar el cubo 1
        GLuint isGRIDLoc = glGetUniformLocation(shaderProgram, "isGRID");
        GLuint cubeColorLoc = glGetUniformLocation(shaderProgram, "cubeColor");
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniform1i(isGRIDLoc, false);
        glUniform3f(cubeColorLoc, 1.0f, 0.0f, 0.0f);
        glm::mat4 model1 = glm::translate(glm::mat4(1.0f), cube1NewPosition);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Block block1(Vector3D(cube1NewPosition.x, cube1NewPosition.y, cube1NewPosition.z), Vector3D(1.0f, 0.0f, 0.0f));


        // Dibujar el cubo 2
        glUniform1i(isGRIDLoc, false);
        glUniform3f(cubeColorLoc, 0.0f, 1.0f, 0.0f);
        glm::mat4 model2 = glm::translate(glm::mat4(1.0f), cube2NewPosition);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        glm::mat4 identityModel = glm::mat4(1.0f);
        glUniform1i(isGRIDLoc, true);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identityModel));
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);
        glBindVertexArray(0);

        // Intercambiar buffers y procesar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpieza de recursos
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}