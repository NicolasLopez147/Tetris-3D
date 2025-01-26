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

class Shader{
    public:
        GLuint ID;

        Shader(const char* vertexSource , const char*fragmentSource){
            ID = createShaderProgram(vertexSource, fragmentSource);
        }
        GLuint getShaderID(){
            return ID;
        }

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

        void use() {
            glUseProgram(ID);
        }

        void setUniformMatrix4fv(const std::string& name, glm::mat4 matrix) const {
            GLuint loc = glGetUniformLocation(ID, name.c_str());
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
        }

        void setUniform3f(const std::string& name, float x, float y, float z) const {
            GLuint loc = glGetUniformLocation(ID, name.c_str());
            glUniform3f(loc, x, y, z);
        }

        void setUniform1i(const std::string& name, int value) const {
            GLuint loc = glGetUniformLocation(ID, name.c_str());
            glUniform1i(loc, value);
        }

};



class Grid{
    GLuint VAO;
    int vertexCount;

    public:
        Grid(int width, int height, int depth){
            std::vector<float> vertices = generateGridVertices(width, height, depth);
            vertexCount = vertices.size() / 3;

            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            GLuint VBO;
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void draw(const Shader& shader) const {
            shader.setUniform1i("isGRID", true);
            glm::mat4 identityModel = glm::mat4(1.0f);
            shader.setUniformMatrix4fv("model", identityModel);
            glBindVertexArray(VAO);
            glDrawArrays(GL_LINES, 0, vertexCount);
            glBindVertexArray(0);
        }
    private:
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

};

class Cube{
    private:
        GLuint VAO, EBO;
        glm::vec3 position;
        glm::vec3 color;
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

    public:
        Cube(glm::vec3 position, glm::vec3 color): position(position), color(color){
            
            GLuint VBO;

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), cubeVertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int), cubeIndices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void draw(const Shader& shader){
            shader.setUniform3f("cubeColor", color.x, color.y, color.z);
            shader.setUniform1i("isGRID", false);

            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            shader.setUniformMatrix4fv("model", model);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        void setPosition(glm::vec3 newPosition){
            position = newPosition;
        }
};

class Render{
    Shader shader;
    Grid grid;
    std::vector<Cube> cubes;
    public:
        Render(const Shader& shader, const Grid& grid): shader(shader), grid(grid){}

        void addCube(const Cube& cube){
            cubes.push_back(cube);
        }

        void render(const glm::mat4& projection, const glm::mat4& view){
            shader.use();
            shader.setUniformMatrix4fv("projection", projection);
            shader.setUniformMatrix4fv("view", view);

            grid.draw(shader);
            for (Cube& cube : cubes){
                cube.draw(shader);
            }
        }
};

class Game{
    private:
        GLFWwindow* window;
        Render renderer;
    public:
        Game(GLFWwindow* window, Render renderer): window(window), renderer(renderer){}

        void run(){
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1600 / 1200, 0.1f, 100.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(15, 25, 15), glm::vec3(5, 10, 5), glm::vec3(0, 1, 0));

            while (!glfwWindowShouldClose(window)) {
                // Limpiar la pantalla
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glEnable(GL_DEPTH_TEST);
                
                renderer.render(projection, view);

                // Intercambiar buffers y procesar eventos
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        }
};

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
    Shader shaderProgram(vertexShaderSource, fragmentShaderSource);

    // Generate grid vertices and set up a VAO/VBO for the grid
    Grid grid(6, 15, 6);
    Render render(shaderProgram, grid);
    render.addCube(Cube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    Cube cube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f));
    cube.setPosition(glm::vec3(0.0f, 10.0f, 1.0f));
    render.addCube(cube);

    Game game(window, render);
    game.run();
    return 0;
}