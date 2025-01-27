#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <random>
#include <algorithm>
// #include <bits/algorithmfwd.h>


// Callback function to adjust the OpenGL viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


class Shader{
    private:
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

        uniform vec3 blockColor; // Color of the Block
        uniform bool isGRID; // Color of the Block

        in vec3 FragPos;  // Position of the fragment
        in float FragHeight; // Height of the fragment

        void main() {
            if(isGRID){
                float alpha = 1.0 - clamp(abs(FragHeight) / 18.0, 0.0, 1.0);  // Calculate transparency based on height
                FragColor = vec4(1.0, 1.0, 1.0, alpha);  // Set the color of the fragment
            }else{
                FragColor = vec4(blockColor,1.0);  // Set the color of the fragment
            }
        }
        )";
    public:
        GLuint ID;

        Shader(){
            ID = createShaderProgram(vertexShaderSource, fragmentShaderSource);
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



class Vector3D {
public:
    // Public member variables representing the 3D coordinates
    float x, y, z;

    // Constructor: Initializes the vector with default or given coordinates
    Vector3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    // Overloaded operator+ to add two vectors component-wise
    Vector3D operator+(const Vector3D& other) const {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }

    // Overloaded operator- to subtract two vectors component-wise
    Vector3D operator-(const Vector3D& other) const {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }

    // Overloaded operator/ to divide the vector components by a scalar
    Vector3D operator/(float scalar) const {
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }

    // Overloaded operator* to multiply the vector components by a scalar
    Vector3D operator*(float scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    // Overloaded operator== to compare two vectors for equality
    bool operator==(const Vector3D& other) {
        return x == other.x && y == other.y && z == other.z;
    }
};


class Block{
    private:
        GLuint VAO, EBO;
        Vector3D positionVector3D;
        Vector3D colorVector3D;
        glm::vec3 position;
        glm::vec3 color;

        std::vector<float> blockVertices = {
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

        std::vector<unsigned int> blockIndices = {
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
        Block(const Vector3D& positionVector3D, const Vector3D& colorVector3D): positionVector3D(positionVector3D), colorVector3D(colorVector3D){
            position = glm::vec3(positionVector3D.x, positionVector3D.y, positionVector3D.z);
            color = glm::vec3(colorVector3D.x, colorVector3D.y, colorVector3D.z);

            GLuint VBO;

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, blockVertices.size() * sizeof(float), blockVertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, blockIndices.size() * sizeof(unsigned int), blockIndices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void draw(const Shader& shader) const {
            shader.setUniform3f("blockColor", color.x, color.y, color.z);
            shader.setUniform1i("isGRID", false);

            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            shader.setUniformMatrix4fv("model", model);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, blockIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        void setPosition(glm::vec3 newPosition){
            position = newPosition;
        }

        glm::vec3 getPosition() const{
            return position;
        }
};

class Tetromino {
private:
    // A collection of blocks that make up the Tetromino
    std::vector<Block> blocks;

    // Color of the Tetromino, applied to all its blocks
    Vector3D color;

    glm::vec3 center;

    // Rotation state of the Tetromino
    glm::mat4 rotation;


    // Maximum value for random color generation (normalized to 1.0f)
    const int rand_max = 255;

    // Generates a random color for the Tetromino
    Vector3D randomColor() {
        float r = static_cast<float>(rand()) / RAND_MAX; // Generate red component
        float g = static_cast<float>(rand()) / RAND_MAX; // Generate green component
        float b = static_cast<float>(rand()) / RAND_MAX; // Generate blue component
        std::cout << "Generated Color: (" << r << ", " << g << ", " << b << ")" << std::endl; // Debug output
        return Vector3D(r, g, b);
    }

    void calculateCenter() {
        std::cout << "Calculating center" << std::endl;
        if (blocks.empty()) return;
        glm::vec3 sum(0, 0, 0);
        for (const auto& block : blocks) {
            sum = sum + block.getPosition();
        }
        center = glm::vec3(
            round(sum.x / blocks.size()),
            round(sum.y / blocks.size()),
            round(sum.z / blocks.size())
        );
        std::cout << "Center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
    }


public:
    Tetromino() {}
    // Constructor: Initializes the Tetromino at a position with a specific shape
    Tetromino(const Vector3D& pos, int shape) : color(randomColor()) , rotation(glm::mat4(1.0f)) {
        setShape(shape);
        calculateCenter();
        move(glm::vec3 (pos.x, pos.y, pos.z)); // Adjust blocks to the initial position
    }

    // Overloaded assignment operator
    Tetromino& operator=(const Tetromino& other) {
        if (this != &other) {
            blocks = other.blocks;
            color = other.color;
            rotation = other.rotation;
        }
        return *this;
    }

    // Adds a block to the Tetromino
    void addBlock(const Block& block) {
        blocks.push_back(block);
    }

    // Sets the Tetromino's shape by adding blocks in a predefined configuration
        void setShape(int shape) {
        switch (shape) {
        case 0: // I-shape
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(2, 0, 0), color));
            addBlock(Block(Vector3D(3, 0, 0), color));
            break;
        case 1: // J-shape
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(1, 1, 0), color));
            addBlock(Block(Vector3D(1, 2, 0), color));
            break;
        case 2: // L-shape
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(0, 1, 0), color));
            addBlock(Block(Vector3D(0, 2, 0), color));
            break;
        case 3: // O-shape
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(0, 1, 0), color));
            addBlock(Block(Vector3D(1, 1, 0), color));
            break;
        case 4: // S-shape
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(0, 1, 0), color));
            addBlock(Block(Vector3D(1, 1, 0), color));
            addBlock(Block(Vector3D(2, 1, 0), color));
            break;
        case 5: // T-shape
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(2, 0, 0), color));
            addBlock(Block(Vector3D(1, 1, 0), color));
            break;
        case 6: // Z-shape
            addBlock(Block(Vector3D(0, 1, 0), color));
            addBlock(Block(Vector3D(1, 1, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(2, 0, 0), color));
            break;
        default:
            break;
        }
    }

    // Moves the Tetromino by a given vector
    void move(glm::vec3 direction) {
        for (auto& block : blocks) {
            block.setPosition(block.getPosition() + direction);
        }
    }

    // Rotates the Tetromino around a specified axis by a given angle
    void rotate(float angle, const glm::vec3& axis) {
        glm::mat4 currentRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
        rotation = currentRotation * rotation;
        applyRotation();
    }

    void applyRotation(){
        calculateCenter();
        for (auto& block : blocks){
            glm::vec4 localPosition = glm::vec4(block.getPosition() - center, 1.0f);
            glm::vec4 rotatedPosition = rotation * localPosition;
            glm::vec3 newPosition = glm::vec3(rotatedPosition) + center;

            newPosition = glm::round(newPosition);

            block.setPosition(newPosition);
        }
    }

    // Draws the Tetromino by drawing each block
    void draw(const Shader& shader) const {
        for (const Block& block : blocks) {
            block.draw(shader);
        }
    }


    // Returns a constant reference to the blocks in the Tetromino
    const std::vector<Block>& getBlocks() const { return blocks; }
};


class Grid{
    private:
        GLuint VAO;
        int vertexCount;
        int width, height, depth;

        std::vector<std::vector<std::vector<bool>>> cells;

        std::vector<int> lineCounters;

    public:
        Grid(){}
        Grid(int width, int height, int depth): width(width), height(height), depth(depth), cells(width, std::vector<std::vector<bool>>(height, std::vector<bool>(depth, false))), lineCounters(height, 0){
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

        // Checks if the given Tetromino collides with the boundaries or occupied cells in the grid
        bool checkCollision(const Tetromino& tetromino) const {
            for (const auto& block : tetromino.getBlocks()) {
                glm::vec3 pos = block.getPosition();
                int x = static_cast<int>(pos.x);
                int y = static_cast<int>(pos.y);
                int z = static_cast<int>(pos.z);

                // Check if the block is outside the grid boundaries
                if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
                    return true;
                }

                // Check if the block is colliding with an occupied cell
                if (cells[x][y][z]) {
                    return true;
                }
            }
            return false;
        }

        void printGrid(){
            for (int y = height - 1; y >= 0; --y) {
                for (int x = 0; x < width; ++x) {
                    std::cout << (cells[x][y][0] ? "X" : ".");
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
            std::cout << "-----------------------------------"<<std::endl;
        }

        // Places the given Tetromino onto the grid and updates the occupied cells and line counters
        void placeTetromino(const Tetromino& tetromino) {
            for (const auto& block : tetromino.getBlocks()) {
                glm::vec3 pos = block.getPosition();
                int x = static_cast<int>(pos.x);
                int y = static_cast<int>(pos.y);
                int z = static_cast<int>(pos.z);

                // Mark the cell as occupied and increment the line counter for the respective y-level
                if (!cells[x][y][z]) {
                    cells[x][y][z] = true;
                    lineCounters[y]++;
                }
            }
        }

        // Clears any fully occupied lines (layers) and shifts the above layers down
        int clearLines() {
            int lines = 0;
            int y = 0;
            while(y < height){
                // Check if the layer is fully occupied
                if (lineCounters[y] == width * depth) {
                    // Clear the layer
                    for (int z = 0; z < depth; ++z) {
                        for (int x = 0; x < width; ++x) {
                            cells[x][y][z] = false;
                        }
                    }

                    // Shift the layers above this one down
                    for (int ny = y; ny < height - 1; ++ny) {
                        for (int z = 0; z < depth; ++z) {
                            for (int x = 0; x < width; ++x) {
                                cells[x][ny][z] = cells[x][ny + 1][z];
                            }
                        }
                        // Update the line counter for the shifted layer
                        lineCounters[ny] = lineCounters[ny + 1];
                    }

                    // Clear the topmost layer
                    lineCounters[height - 1] = 0;
                    lines+=1;
                    y--;
                }
                y++;
            }
            return lines;
        }

        void draw(const Shader& shader) const {
            shader.setUniform1i("isGRID", true);
            glm::mat4 identityModel = glm::mat4(1.0f);
            shader.setUniformMatrix4fv("model", identityModel);
            glBindVertexArray(VAO);
            glDrawArrays(GL_LINES, 0, vertexCount);
            glBindVertexArray(0);
        }

        bool isCellOccupied(int x, int y, int z) const {
            return cells[x][y][z];
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


class Render{
    Shader shader;
    Grid grid;
    std::vector<Block> blocks;
    std::vector<Block> activeTetrominoBlocks;
    std::vector<Block> nextTetrominoBlocks;
    public:
        Render(): shader() {}

        void addBlock(const Block& block){
            blocks.push_back(block);
        }

        void setActiveTetromino(const Tetromino& tetromino){
            activeTetrominoBlocks.clear();
            for (const Block& block : tetromino.getBlocks()){
                activeTetrominoBlocks.push_back(block);
            }
        }

        void setNextTetromino(const Tetromino& tetromino){
            nextTetrominoBlocks.clear();
            for (const Block& block : tetromino.getBlocks()){
                nextTetrominoBlocks.push_back(block);
            }
        }

        void addTetromino(const Tetromino& tetromino){
            for (const Block& block : tetromino.getBlocks()){
                addBlock(block);
            }
        }

        void setGrille(int width, int height, int depth){
            grid = Grid(width, height, depth);
        }

        void removeClearedBlocks(const Grid& grid) {
            // Filtrar solo los bloques que aún están marcados en la grilla
            blocks.erase(
                std::remove_if(blocks.begin(), blocks.end(),
                    [&grid](const Block& block) {
                        glm::vec3 pos = block.getPosition();
                        int x = static_cast<int>(pos.x);
                        int y = static_cast<int>(pos.y);
                        int z = static_cast<int>(pos.z);
                        // Eliminar el bloque si la celda correspondiente no está ocupada
                        return !grid.isCellOccupied(x, y, z);
                    }
                ),
                blocks.end()
            );
        }


        void render(const glm::mat4& projection, const glm::mat4& view){
            shader.use();
            shader.setUniformMatrix4fv("projection", projection);
            shader.setUniformMatrix4fv("view", view);
            grid.draw(shader);
            for (Block& block : blocks){
                block.draw(shader);
            }

            for (Block& block : activeTetrominoBlocks){
                block.draw(shader);
            }

            for (Block& block : nextTetrominoBlocks){
                block.draw(shader);
            }
        }
};

class Game{
    private:
        GLFWwindow* window;
        Render renderer;

        Grid grid;
        Tetromino currentTetromino;
        Tetromino nextTetromino;
        bool isRunning;
        int score;
        int level;
        int linesCleared;
        float fallSpeed;
        int WIDTH = 6;
        int HEIGHT = 16;
        int DEPTH = 6;

        int nextShape;

        const int LINES_PER_LEVEL = 10;
        const float SPEED_INCREMENT = 0.1f;
        const Vector3D POSITION_NEW_TETROMINO = Vector3D(WIDTH/2, HEIGHT, DEPTH/2);
        const Vector3D POSITION_NEXT_TETROMINO = Vector3D(WIDTH + 3, HEIGHT/2, 0);

        int setShape(){
            // std::random_device rd;
            // std::mt19937 gen(rd());
            // std::uniform_int_distribution<> dist(0, 6);
            // return dist(gen);
            return 3;
        }

        void checkPositionTetromino(Tetromino& tetromino){
            for (const auto& block : tetromino.getBlocks()){
                glm::vec3 blockPos = block.getPosition();
                if (blockPos.y >= HEIGHT){
                    tetromino.move(glm::vec3(0, -(blockPos.y - HEIGHT + 1), 0));
                }else if(blockPos.y < 0){
                    tetromino.move(glm::vec3(0, -blockPos.y, 0));
                }
                if (blockPos.x >= WIDTH){
                    tetromino.move(glm::vec3(-(blockPos.x - WIDTH + 1),0,0));
                }else if (blockPos.x < 0){
                    tetromino.move(glm::vec3(-blockPos.x,0,0));
                }
                if (blockPos.z >= DEPTH){
                    tetromino.move(glm::vec3(0,0,-(blockPos.z - DEPTH + 1)));
                } else if (blockPos.z < 0){
                    tetromino.move(glm::vec3(0,0,-blockPos.z));
                }
            }
        }

    public:
        Game(GLFWwindow* window): renderer(), window(window) {
            start();
        }

        void start(){
            isRunning = true;
            score = 0;
            level = 1;
            linesCleared = 0;
            fallSpeed = 1.0f;
            grid = Grid(WIDTH, HEIGHT, DEPTH);
            nextShape = setShape();
            currentTetromino = Tetromino(POSITION_NEW_TETROMINO, setShape());
            checkPositionTetromino(currentTetromino);
            nextTetromino = Tetromino(POSITION_NEXT_TETROMINO, nextShape);
            renderer.setGrille(WIDTH, HEIGHT, DEPTH);
        }

        void run(){
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1600 / 1200, 0.1f, 100.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(15, 25, 15), glm::vec3(5, 10, 5), glm::vec3(0, 1, 0));

            while (!glfwWindowShouldClose(window)) {
                // Limpiar la pantalla
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glEnable(GL_DEPTH_TEST);

                if (isRunning) {
                    update(0.005f);
                    renderer.render(projection, view);
                }
                

                // Intercambiar buffers y procesar eventos
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        }

        void update(float deltaTime) {
            static float accumulatedTime = 0.0f;
            accumulatedTime += deltaTime;

            if (accumulatedTime >= fallSpeed) {
                // Move the current Tetromino down
                currentTetromino.move(glm::vec3(0, -1, 0));
                std::cout << "Current Tetromino moved down" << std::endl;

                
                // Place the Tetromino and clear lines if it collides
                if (grid.checkCollision(currentTetromino)) {
                    currentTetromino.move(glm::vec3(0, 1, 0)); // Undo the move
                    grid.placeTetromino(currentTetromino);
                    linesCleared += grid.clearLines();

                    

                    // Level up if enough lines are cleared
                    if (linesCleared % LINES_PER_LEVEL == 0) {
                        level++;
                        linesCleared = 0;
                        fallSpeed = std::max(fallSpeed - SPEED_INCREMENT, 0.2f); // Limit minimum speed
                    }
                    renderer.addTetromino(currentTetromino);

                    renderer.removeClearedBlocks(grid);

                    // Set up the next Tetromino
                    currentTetromino = Tetromino(POSITION_NEW_TETROMINO, nextShape);
                    checkPositionTetromino(currentTetromino);
                    nextShape = setShape();
                    nextTetromino = Tetromino(POSITION_NEXT_TETROMINO, nextShape);

                    // Check if the game is over
                    isRunning = !checkGameOver(currentTetromino);
                }

                accumulatedTime = 0.0f;
            }

            renderer.setActiveTetromino(currentTetromino);
            renderer.setNextTetromino(nextTetromino);
        }

        void moveTetromino(const Vector3D& direction){
            currentTetromino.move(glm::vec3(direction.x, direction.y, direction.z));
            if (grid.checkCollision(currentTetromino)){
                currentTetromino.move(glm::vec3(-direction.x, -direction.y, -direction.z));
            }
        }

        void rotateTetromino(float angle, const Vector3D& axis){
            currentTetromino.rotate(angle, glm::vec3(axis.x, axis.y, axis.z));
            checkPositionTetromino(currentTetromino);
            if (grid.checkCollision(currentTetromino)){
                std::cout << "Collision detected" << std::endl;
                currentTetromino.rotate(-angle, glm::vec3(axis.x, axis.y, axis.z));
            }
        }

        int getScore() const{
            return score;
        }

        int getLinesCleared() const{
            return linesCleared;
        }

        bool getIsRunning() const{
            return isRunning;
        }

        bool checkGameOver(Tetromino currentTetromino) const{
            return grid.checkCollision(currentTetromino);
        }
};

class InputHandler {
public:
    // Processes user input and performs the corresponding actions on the game
    void handleInput(int key, Game& game) {
    switch (key) {
    case GLFW_KEY_S: // Move the current Tetromino down
        game.moveTetromino(Vector3D(0, -1, 0));
        break;

    case GLFW_KEY_A: // Move the current Tetromino left
        game.moveTetromino(Vector3D(-1, 0, 0));
        break;

    case GLFW_KEY_D: // Move the current Tetromino right
        game.moveTetromino(Vector3D(1, 0, 0));
        break;
    
    case GLFW_KEY_Q: // Move the current Tetromino up
        game.moveTetromino(Vector3D(0, 0, -1));
        break;

    case GLFW_KEY_E: // Rotate the Tetromino 90 degrees around the X-axis
        game.moveTetromino(Vector3D(0, 0, 1));
        break;

    case GLFW_KEY_Z: // Rotate the Tetromino 90 degrees around the Z-axis
        game.rotateTetromino(90.0f, Vector3D(0, 0, 1));
        break;
    case GLFW_KEY_X: // Rotate the Tetromino 90 degrees around the X-axis
        game.rotateTetromino(90.0f, Vector3D(1, 0, 0));
        break;
    case GLFW_KEY_C: // Rotate the Tetromino 90 degrees around the Y-axis
        game.rotateTetromino(90.0f, Vector3D(0, 1, 0));
        break;

    default:
        // Optional: Handle invalid keys or no-op
        break;
    }
}

};

InputHandler inputHandler;

// Callback for GLFW to capture key input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        Game* game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
        if (game) {
            inputHandler.handleInput(key, *game);
        }
    }
}


#include "./src/fontworks.cpp" // Ensure the refactored version is used


// Enumeration to define the game states
enum GameState {
    Menu,
    Playing,
    GameOver
};

// Global or static variable to track the falling offset
float fallingOffset = 0.0f;

// Function to draw the title with a falling effect for the letter 'S'
void drawTitle(TextShader& textShader, float windowWidth, float windowHeight) {
    // Define the position and size for "TETRIS"
    float size = 1.5f;
    float titleX = (windowWidth / 2) - 120.0f; // Center the title
    float titleY = windowHeight - 100.0f;     // Position near the top
    std::vector<glm::vec3> colors = {
        glm::vec3(1.0f, 0.0f, 0.0f), // T: Red
        glm::vec3(1.0f, 0.5f, 0.0f), // E: Orange
        glm::vec3(1.0f, 1.0f, 0.0f), // T: Yellow
        glm::vec3(0.0f, 1.0f, 0.0f), // R: Green
        glm::vec3(0.0f, 1.0f, 1.0f), // I: Cyan color
        glm::vec3(1.0f, 0.0f, 1.0f)  // S: Magenta (Fuchsia)
    };

    // Render each letter of "TETRIS"
    std::string title = "TETRIS";
    for (size_t i = 0; i < title.size(); ++i) {
        if (title[i] == 'S') {
            // Apply falling effect for the letter 'S'
            float letterX = titleX + i * 40.0f; // Calculate X position
            float letterY = titleY - fallingOffset; // Apply fallingOffset to Y position

            // Render the falling 'S'
            renderText(textShader, std::string(1, title[i]), letterX, letterY, size, colors[i]);

            // Reset or loop the falling effect
            if (fallingOffset > 100.0f) { // If it falls beyond a threshold
                fallingOffset = 0.0f;     // Reset to the starting position
            } else {
                fallingOffset += 0.5f;    // Increment the offset (controls speed)
            }
        } else {
            // Render other letters without animation
            renderText(textShader, std::string(1, title[i]), titleX + i * 40.0f, titleY, size, colors[i]);
        }
    }

    // Render "3D" below "TETRIS" in bold white color
    renderText(textShader, "3D", titleX + 80.0f, titleY - 60.0f, size, glm::vec3(1.0f, 1.0f, 1.0f));
}


// Function to draw the footer
void drawFooter(TextShader& textShader, float windowWidth) {
    float footerX = (windowWidth / 2) - 180.0f; // Centered horizontally
    float footerY = 20.0f;                     // Position near the bottom
    renderText(textShader, "Copyright: Nicolas LOPEZ and Nicolas RINCON", footerX, footerY, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f)); // White color
}


// Function to check if a mouse click is within a button's boundaries
bool isMouseOverButton(double mouseX, double mouseY, float buttonX, float buttonY, float buttonWidth, float buttonHeight) {
    return mouseX >= buttonX && mouseX <= buttonX + buttonWidth && mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
}

// Function to draw a button with hover effect
void drawButton(TextShader& textShader, float x, float y, float width, float height, const std::string& text, glm::vec3 textColor, bool isHovered) {
    // Draw the button rectangle (background)
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.2f, 0.2f); // Button background color
    glVertex2f(x, y);                // Bottom-left
    glVertex2f(x + width, y);        // Bottom-right
    glVertex2f(x + width, y + height); // Top-right
    glVertex2f(x, y + height);       // Top-left
    glEnd();

    // Change text color if hovered
    glm::vec3 finalTextColor = isHovered ? glm::vec3(1.0f, 0.8f, 0.0f) : textColor; // Hover color is orange
    float textX = x + (width / 2) - (text.size() * 10.0f) / 2; // Center horizontally
    float textY = y + (height / 2) - 10.0f; // Center vertically
    renderText(textShader, text, textX, textY, 1.0f, finalTextColor);
}

// Function to display the menu
void displayMenu(GLFWwindow* window, GameState &state) {
    // Initialize a TextShader instance
    TextShader textShader;
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f); // Flip the Y-axis
    textShader.use();
    textShader.setMat4("projection", projection);

    // Get window dimensions dynamically
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Button properties (centered)
    float buttonWidth = 200.0f, buttonHeight = 50.0f;
    float startX = (windowWidth / 2) - 120.0f;
    float startY = windowHeight / 2 - 50.0f;  // "START" button
    float quitX = startX;                     // Same X as "START"
    float quitY = startY - 80.0f;             // "QUIT" button below "START"

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouseY = windowHeight - mouseY; // Flip Y-axis to align with OpenGL

    // Check hover state for each button
    bool startHovered = isMouseOverButton(mouseX, mouseY, startX, startY, buttonWidth, buttonHeight);
    bool quitHovered = isMouseOverButton(mouseX, mouseY, quitX, quitY, buttonWidth, buttonHeight);

    // Render the title
    drawTitle(textShader, windowWidth, windowHeight);

    // Render menu buttons with hover effect
    drawButton(textShader, startX, startY, buttonWidth, buttonHeight, "START", glm::vec3(1.0f, 1.0f, 1.0f), startHovered);
    drawButton(textShader, quitX, quitY, buttonWidth, buttonHeight, "QUIT", glm::vec3(1.0f, 1.0f, 1.0f), quitHovered);

    // Render the footer
    drawFooter(textShader, windowWidth);

    // Check for mouse clicks
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (startHovered) {
            state = Playing; // Transition to the Playing state
        }
        if (quitHovered) {
            glfwSetWindowShouldClose(window, true); // Close the window
        }
    }
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Tetris 3D", nullptr, nullptr);
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
    glEnable(GL_DEPTH_TEST);

    // Set the initial game state
    GameState state = Menu;
    Game game(window);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        switch (state) {
            case Menu:
                displayMenu(window, state);
                break;
            case Playing:
                game.run(); // Start the game
                state = GameOver; // Temporary, for when the game ends
                break;
            case GameOver:
                // Implement a game over screen or restart logic if needed
                glfwSetWindowShouldClose(window, true);
                break;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

