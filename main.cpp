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
#include "./src/Shader.h"
#include "./src/TextShader.h"

// Callback function to adjust the OpenGL viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

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
        Vector3D positionVector3D;
        Vector3D colorVector3D;
        glm::vec3 position;
        glm::vec3 color;
        GLuint sharedVAO, sharedVBO, sharedEBO;

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
        Block(const Vector3D& positionVector3D, const Vector3D& colorVector3D): positionVector3D(positionVector3D), colorVector3D(colorVector3D), sharedVAO(0), sharedVBO(0), sharedEBO(0) {
            position = glm::vec3(positionVector3D.x, positionVector3D.y, positionVector3D.z);
            color = glm::vec3(colorVector3D.x, colorVector3D.y, colorVector3D.z);

            initializeSharedResources();
        }

        glm::vec3 getColor() const {
            return color;
        }

        void initializeSharedResources() {
            if (sharedVAO == 0) { // Solo inicializar una vez
                glGenVertexArrays(1, &sharedVAO);
                glGenBuffers(1, &sharedVBO);
                glGenBuffers(1, &sharedEBO);

                glBindVertexArray(sharedVAO);

                glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
                glBufferData(GL_ARRAY_BUFFER, blockVertices.size() * sizeof(float), blockVertices.data(), GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, blockIndices.size() * sizeof(unsigned int), blockIndices.data(), GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
        }

        void draw(const Shader& shader) const {
            shader.setUniform3f("blockColor", color.x, color.y, color.z);
            shader.setUniform1i("isGRID", false);

            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            shader.setUniformMatrix4fv("model", model);
            glBindVertexArray(sharedVAO);
            glDrawElements(GL_TRIANGLES, blockIndices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        void setPosition(glm::vec3 newPosition){
            position = newPosition;
        }

        glm::vec3 getPosition() const{
            return position;
        }

        void cleanUp() {
            glDeleteVertexArrays(1, &sharedVAO);
            glDeleteBuffers(1, &sharedVBO);
            glDeleteBuffers(1, &sharedEBO);
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
        return Vector3D(r, g, b);
    }

    void calculateCenter() {
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
    }


public:
    Tetromino() {}
    // Constructor: Initializes the Tetromino at a position with a specific shape
    Tetromino(const Vector3D& pos, int shape) : color(randomColor()) , rotation(glm::mat4(1.0f)) {
        setShape(shape);
        calculateCenter();
        move(glm::vec3 (pos.x, pos.y, pos.z)); // Adjust blocks to the initial position
    }

    Tetromino(const Vector3D& pos, int shape, const Vector3D& col) : color(col) , rotation(glm::mat4(1.0f)) {
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

    Vector3D getColor(){
        return color;
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
        std::vector<std::vector<std::vector<glm::vec3>>> cellColors;


        std::vector<int> lineCounters;

    public:
        Grid(){}
        Grid(int width, int height, int depth): width(width), height(height), depth(depth), cells(width, std::vector<std::vector<bool>>(height, std::vector<bool>(depth, false))), lineCounters(height, 0), cellColors(width, std::vector<std::vector<glm::vec3>>(height, std::vector<glm::vec3>(depth, glm::vec3(0.0f)))) {
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

        glm::vec3 getCellColor(int x, int y, int z) const {
            return cellColors[x][y][z];
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

        void cleanUp() {
            glDeleteVertexArrays(1, &VAO);
        }

        int getWidth() const { return width; }
        int getHeight() const { return height; }
        int getDepth() const { return depth; }

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
                    cellColors[x][y][z] = block.getColor();
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
    TextShader textShader;
    Grid grid;
    std::vector<Block> blocks;
    std::vector<Block> activeTetrominoBlocks;
    std::vector<Block> nextTetrominoBlocks;
    std::vector<Block> projectedTetrominoBlocks;
    public:
        Render(): shader() {
        }

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

        void setProjectedTetromino(const Tetromino& tetromino){
            projectedTetrominoBlocks.clear();
            for (const Block& block : tetromino.getBlocks()){
                projectedTetrominoBlocks.push_back(block);
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


        void render(const glm::mat4& projection, const glm::mat4& view, int score, int level) {
            
            
            glm::mat4 projectionText = glm::ortho(0.0f, 1600.0f, 0.0f, 1200.0f);
            textShader.use();
            textShader.setMat4("projection", projectionText);
            std::string scoreText = "Score: " + std::to_string(score);
            std::string levelText = "Level: " + std::to_string(level);
            textShader.renderText(levelText, 1200.0f, 1000.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
            textShader.renderText(scoreText, 1200.0f, 1100.0f, 1.0f, glm::vec3(0.8f, 0.2f, 0.2f));

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

            for (Block& block : projectedTetrominoBlocks){
                block.draw(shader);
            }
        }
    void cleanUp() {
        grid.cleanUp();
        for (Block& block : blocks) {
            block.cleanUp();
        }
        shader.cleanUp();
    }
};

class Game{
    private:
        // GLFWwindow* window;
        // Render renderer;

        Grid grid;
        Tetromino currentTetromino;
        Tetromino nextTetromino;
        bool isRunning;
        int score;
        int level;
        int linesCleared;
        int linesClearedTotal;
        float fallSpeed;
        int WIDTH = 4;
        int HEIGHT = 16;
        int DEPTH = 4;

        int nextShape;

        const int LINES_PER_LEVEL = 10;
        const float SPEED_INCREMENT = 0.1f;
        const Vector3D POSITION_NEW_TETROMINO = Vector3D(WIDTH/2, HEIGHT, DEPTH/2);
        const Vector3D POSITION_NEXT_TETROMINO = Vector3D(WIDTH + 3, HEIGHT/2, 0);
        const float INITIAL_FALL_SPEED = 0.8f;

        int setShape(){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, 6);
            return dist(gen);
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

        Tetromino calculateProjection(const Tetromino& tetromino) const {
            Tetromino projectedTetromino = tetromino;

            while (!grid.checkCollision(projectedTetromino)){
                projectedTetromino.move(glm::vec3(0, -1, 0));
            }

            projectedTetromino.move(glm::vec3(0, 1, 0));

            return projectedTetromino;
        }

    public:
        Game(int width, int height, int depth): grid(width, height, depth), WIDTH(width) , HEIGHT(height), DEPTH(depth){
            start();
        }

        void start(){
            isRunning = true;
            score = 0;
            level = 0;
            linesCleared = 0;
            linesClearedTotal = 0;
            fallSpeed = INITIAL_FALL_SPEED;
            grid = Grid(WIDTH, HEIGHT, DEPTH);
            nextShape = setShape();
            currentTetromino = Tetromino(POSITION_NEW_TETROMINO, setShape());
            checkPositionTetromino(currentTetromino);
            nextTetromino = Tetromino(POSITION_NEXT_TETROMINO, nextShape);
            // renderer.setGrille(WIDTH, HEIGHT, DEPTH);
        }

        // void run() {
        //     glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1600 / 1200, 0.1f, 100.0f);
        //     glm::mat4 view = glm::lookAt(glm::vec3(15, 25, 15), glm::vec3(5, 10, 5), glm::vec3(0, 1, 0));

        //     while (!glfwWindowShouldClose(window) && isRunning) {
        //         // Clear the screen
        //         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //         glEnable(GL_DEPTH_TEST);

        //         if (isRunning) {
        //             update(0.005f);
        //             renderer.render(projection, view,score, level);
        //         }

                
                

        //         // Intercambiar buffers y procesar eventos
        //         glfwSwapBuffers(window);
        //         glfwPollEvents();
        //     }

        //     // Ensure OpenGL state is reset
        //     glDisable(GL_DEPTH_TEST);
        //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // }

        void update(float deltaTime) {
            static float accumulatedTime = 0.0f;
            accumulatedTime += deltaTime;

            fallSpeed = std::max( INITIAL_FALL_SPEED - ((INITIAL_FALL_SPEED / 15) * level), 0.01f);

            if (accumulatedTime >= fallSpeed) {
                // Move the current Tetromino down
                currentTetromino.move(glm::vec3(0, -1, 0));
                
                // Place the Tetromino and clear lines if it collides
                if (grid.checkCollision(currentTetromino)) {
                    currentTetromino.move(glm::vec3(0, 1, 0)); // Undo the move
                    grid.placeTetromino(currentTetromino);
                    linesCleared += grid.clearLines();
                    if (linesCleared == 1){
                        score += 40 * (level + 1);
                    } else if (linesCleared == 2){
                        score += 100 * (level + 1);
                    } else if (linesCleared == 3){
                        score += 300 * (level + 1);
                    } else if (linesCleared == 4){
                        score += 1200 * (level + 1);
                    }
                    linesClearedTotal += linesCleared;
                    linesCleared = 0;
                    level = linesClearedTotal/LINES_PER_LEVEL; ;


                    // renderer.addTetromino(currentTetromino);

                    // renderer.removeClearedBlocks(grid);

                    // Set up the next Tetromino
                    currentTetromino = Tetromino(POSITION_NEW_TETROMINO, nextShape,nextTetromino.getColor());
                    checkPositionTetromino(currentTetromino);
                    nextShape = setShape();
                    nextTetromino = Tetromino(POSITION_NEXT_TETROMINO, nextShape);

                    // Check if the game is over
                    isRunning = !checkGameOver(currentTetromino);
                }

                accumulatedTime = 0.0f;
            }

            // renderer.setActiveTetromino(currentTetromino);
            // renderer.setNextTetromino(nextTetromino);
            // renderer.setProjectedTetromino(calculateProjection(currentTetromino));
        }

        Tetromino getProjectedTetromino(const Tetromino& tetromino) const {
            return calculateProjection(tetromino);
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
                currentTetromino.rotate(-angle, glm::vec3(axis.x, axis.y, axis.z));
            }
        }

        void moveTetrominoToProjectedPosition(){
            currentTetromino = calculateProjection(currentTetromino);
        }

        int getScore() const{
            return score;
        }

        int getTotalLinesCleared() const{
            return linesClearedTotal;
        }

        bool getIsRunning() const{
            return isRunning;
        }

        bool checkGameOver(Tetromino currentTetromino) const{
            return grid.checkCollision(currentTetromino);
        }

        Grid getGrid() const{
            return grid;
        }

        Tetromino getCurrentTetromino() const{
            return currentTetromino;
        }

        Tetromino getNextTetromino() const{
            return nextTetromino;
        }

        int getLevel() const{
            return level;
        }

    public:
        ~Game(){
            grid.cleanUp();
            // renderer.cleanUp();
        }
};

class Renderer {
    private:
        Shader blockShader;
        TextShader textShader;
    public:
        Renderer(): blockShader(), textShader() {}

        void renderGame(const Game& game, const glm::mat4& projection, const glm::mat4& view) {
            // Renderizar la grilla
            renderGrid(game.getGrid(), projection, view);

            renderBlocksInGrille(game.getGrid(), projection, view);

            // Renderizar el Tetromino actual
            renderTetromino(game.getCurrentTetromino(), projection, view);

            // Renderizar el siguiente Tetromino
            renderTetromino(game.getNextTetromino(), projection, view);

            // Renderizar el Tetromino proyectado
            renderTetromino(game.getProjectedTetromino(game.getCurrentTetromino()), projection, view);

            // Renderizar puntaje y nivel
            renderText("Score: " + std::to_string(game.getScore()), 1200.0f, 1100.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            renderText("Level: " + std::to_string(game.getLevel()), 1200.0f, 1000.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        GLuint cubeVAO = 0, cubeVBO = 0, cubeEBO = 0;
        int cubeIndexCount = 36; // 6 caras * 2 triángulos por cara * 3 vértices por triángulo

        void initializeCubeVAO() {
            if (cubeVAO == 0) {
                // Vértices e índices del cubo
                float vertices[] = {
                    // Posición
                    0.0f, 0.0f, 0.0f, // 0
                    1.0f, 0.0f, 0.0f, // 1
                    1.0f, 1.0f, 0.0f, // 2
                    0.0f, 1.0f, 0.0f, // 3
                    0.0f, 0.0f, 1.0f, // 4
                    1.0f, 0.0f, 1.0f, // 5
                    1.0f, 1.0f, 1.0f, // 6
                    0.0f, 1.0f, 1.0f  // 7
                };

                unsigned int indices[] = {
                    // Cara trasera
                    0, 1, 2, 2, 3, 0,
                    // Cara delantera
                    4, 5, 6, 6, 7, 4,
                    // Cara izquierda
                    0, 3, 7, 7, 4, 0,
                    // Cara derecha
                    1, 2, 6, 6, 5, 1,
                    // Cara inferior
                    0, 1, 5, 5, 4, 0,
                    // Cara superior
                    3, 2, 6, 6, 7, 3
                };

                glGenVertexArrays(1, &cubeVAO);
                glGenBuffers(1, &cubeVBO);
                glGenBuffers(1, &cubeEBO);

                glBindVertexArray(cubeVAO);

                glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
        }

        void renderBlocksInGrille(const Grid& grid, const glm::mat4& projection, const glm::mat4& view) {
            initializeCubeVAO();
            blockShader.use();
            blockShader.setUniformMatrix4fv("projection", projection);
            blockShader.setUniformMatrix4fv("view", view);

            for (int x = 0; x < grid.getWidth(); ++x) {
                for (int y = 0; y < grid.getHeight(); ++y) {
                    for (int z = 0; z < grid.getDepth(); ++z) {
                        if (grid.isCellOccupied(x, y, z)) {
                            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                            blockShader.setUniformMatrix4fv("model", model);

                            glm::vec3 color = grid.getCellColor(x, y, z); 
                            std::cout << "Rendering block at (" << x << ", " << y << ", " << z << ") with color: " << color.r << ", " << color.g << ", " << color.b << std::endl;

                            blockShader.setUniform3f("blockColor", color.x, color.y, color.z);

                            // Renderizar un cubo en la posición actual
                            glBindVertexArray(cubeVAO); // Asegúrate de usar un VAO compartido o inicializado para cubos
                            glDrawElements(GL_TRIANGLES, cubeIndexCount, GL_UNSIGNED_INT, 0);
                            glBindVertexArray(0);
                        }
                    }
                }
            }
        }

        void renderTetromino(const Tetromino& tetromino, const glm::mat4& projection, const glm::mat4& view) {
            blockShader.use();
            blockShader.setUniformMatrix4fv("projection", projection);
            blockShader.setUniformMatrix4fv("view", view);

            for (const Block& block : tetromino.getBlocks()) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(block.getPosition()));
                blockShader.setUniformMatrix4fv("model", model);
                block.draw(blockShader);
            }
        }

        void renderGrid(const Grid& grid, const glm::mat4& projection, const glm::mat4& view) {
            blockShader.use();
            blockShader.setUniformMatrix4fv("projection", projection);
            blockShader.setUniformMatrix4fv("view", view);

            // Renderizar la grilla aquí
            grid.draw(blockShader);

        }

        void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color) {
            textShader.use();
            textShader.setVec3("textColor", color);
            textShader.renderText(text, x, y, scale, color);
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
    case GLFW_KEY_SPACE: // Restart the game
        game.moveTetrominoToProjectedPosition();
        break;
    default:
        // Optional: Handle invalid keys or no-op
        break;
    }
}

};

InputHandler inputHandler;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        Game* game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
        if (game) {
            inputHandler.handleInput(key, *game);
        }
    }
}

// Enumeration to define the game states
enum GameState {
    MenuPrincipal,
    Playing,
    HowToPlay,
    GameOver
};

class Menu {
public:
    Menu(GLFWwindow* window, GameState& state)
        : window(window), state(state), textShader() {
        projection = glm::ortho(0.0f, 1600.0f, 0.0f, 1200.0f);
    }

    void displayMenu() {
        textShader.use();
        textShader.setMat4("projection", projection);

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseY = windowHeight - mouseY;

        // Calculate button positions
        float startX = (windowWidth / 2) - 120.0f;
        float startY = windowHeight - 600.0f;
        float howToPlayX = startX - 50.0f;
        float howToPlayY = startY - 80.0f;
        float quitX = startX;
        float quitY = howToPlayY - 80.0f;

        // Check hover states
        bool startHovered = isMouseOverButton(mouseX, mouseY, startX, startY, buttonWidth, buttonHeight);
        bool howToPlayHovered = isMouseOverButton(mouseX, mouseY, howToPlayX, howToPlayY, buttonWidth, buttonHeight);
        bool quitHovered = isMouseOverButton(mouseX, mouseY, quitX, quitY, buttonWidth, buttonHeight);

        // Draw elements
        drawTitle(windowWidth, windowHeight);
        drawButton(startX, startY, buttonWidth, buttonHeight, "START", glm::vec3(1.0f, 1.0f, 1.0f), startHovered);
        drawButton(howToPlayX, howToPlayY, buttonWidth, buttonHeight, "HOW TO PLAY", glm::vec3(1.0f, 1.0f, 1.0f), howToPlayHovered);
        drawButton(quitX, quitY, buttonWidth, buttonHeight, "QUIT", glm::vec3(1.0f, 1.0f, 1.0f), quitHovered);
        drawFooter(windowWidth);

        // Handle click events
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (startHovered) {
                state = Playing;
            }
            if (howToPlayHovered) {
                state = HowToPlay;
            }
            if (quitHovered) {
                textShader.cleanup();
                glfwSetWindowShouldClose(window, true);
            }
        }
    }

private:
    GLFWwindow* window;
    GameState& state;
    TextShader textShader;
    glm::mat4 projection;
    float fallingOffset = 0.0f;
    const float buttonWidth = 200.0f, buttonHeight = 50.0f;

    void drawTitle(float windowWidth, float windowHeight) {
        float size = 3.5f;
        float titleX = (windowWidth / 2) - 350.0f;
        float titleY = windowHeight - 120.0f;
        std::vector<glm::vec3> colors = {
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.5f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 0.0f, 1.0f)
        };

        std::string title = "TETRIS";
        for (size_t i = 0; i < title.size(); ++i) {
            float letterX = titleX + i * 120.0f;
            float letterY = (title[i] == 'S') ? titleY - fallingOffset : titleY;
            textShader.renderText(std::string(1, title[i]), letterX, letterY, size, colors[i]);

            if (title[i] == 'S') {
                fallingOffset = (fallingOffset > 100.0f) ? 0.0f : fallingOffset + 0.5f;
            }
        }
        textShader.renderText("3D", titleX + 270.0f, titleY - 190.0f, size, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    void drawFooter(float windowWidth) {
        float footerX = (windowWidth / 2) - 180.0f;
        float footerY = 20.0f;
        textShader.renderText("Copyright: Nicolas LOPEZ and Nicolas RINCON", footerX, footerY, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    bool isMouseOverButton(double mouseX, double mouseY, float buttonX, float buttonY, float buttonWidth, float buttonHeight) {
        return mouseX >= buttonX && mouseX <= buttonX + buttonWidth && mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
    }

    void drawButton(float x, float y, float width, float height, const std::string& text, glm::vec3 textColor, bool isHovered) {
        glBegin(GL_QUADS);
        glColor3f(0.2f, 0.2f, 0.2f);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();

        glm::vec3 finalTextColor = isHovered ? glm::vec3(1.0f, 0.8f, 0.0f) : textColor;
        float textX = x + (width / 2) - (text.size() * 10.0f) / 2;
        float textY = y + height+(height/2);
        textShader.renderText(text, textX, textY, 1.0f, finalTextColor);
    }
};

class HowToPlayScreen {
public:
    HowToPlayScreen(GLFWwindow* window, GameState& state)
        : window(window), state(state), textShader() {
        projection = glm::ortho(0.0f, 1600.0f, 0.0f, 1200.0f);
        textShader.use();
        textShader.setMat4("projection", projection);
    }

    void display() {
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseY = windowHeight - mouseY; // Adjust for OpenGL coordinates

        // Check hover state for the "RETURN TO MENU" button
        bool returnHovered = isMouseOverButton(mouseX, mouseY, windowWidth / 2 - 100.0f, 50.0f, 200.0f, 50.0f);

        // Draw instructions
        drawInstructions(windowWidth, windowHeight, returnHovered);

        // Handle click on "RETURN TO MENU"
        if (returnHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            state = MenuPrincipal; // Transition to the main menu
        }
    }

private:
    GLFWwindow* window;
    GameState& state;
    TextShader textShader;
    glm::mat4 projection;

    void drawInstructions(float windowWidth, float windowHeight, bool returnHovered) {
        float introX = windowWidth / 2 - 200.0f;
        float introY = windowHeight - 100.0f;
        textShader.renderText("Welcome to Tetris 3D!", introX, introY, 0.9f, glm::vec3(1.0f, 1.0f, 1.0f));
        textShader.renderText("Use the following keys to play the game:", introX, introY - 30.0f, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f));

        std::vector<std::pair<std::string, std::vector<std::string>>> groupedInstructions = {
            {"Movement Controls:", {
                "Key S: Move down",
                "Key A: Move left X-axis",
                "Key D: Move right X-axis",
                "Key Q: Move left Z-axis",
                "Key E: Rotate right Z-axis",
                "Key  SPACE: Move to projected position",
            }},
            {"Rotation Controls:", {
                "Key Z: Rotate around Z-axis",
                "Key X: Rotate around X-axis",
                "Key C: Rotate around Y-axis"
            }}
        };

        float x = windowWidth / 2 - 200.0f;
        float y = introY - 80.0f;
        float groupSpacing = 35.0f;
        float lineSpacing = 30.0f;

        for (const auto& group : groupedInstructions) {
            textShader.renderText(group.first, x, y, 0.7f, glm::vec3(1.0f, 1.0f, 0.0f));
            y -= groupSpacing;

            for (const auto& instruction : group.second) {
                textShader.renderText(instruction, x + 20.0f, y, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
                y -= lineSpacing;
            }

            y -= groupSpacing;
        }

        float buttonX = windowWidth / 2 - 100.0f;
        float buttonY = 50.0f;
        glm::vec3 buttonColor = returnHovered ? glm::vec3(1.0f, 0.8f, 0.0f) : glm::vec3(1.0f, 1.0f, 0.0f);
        textShader.renderText("RETURN TO MENU", buttonX, buttonY, 0.8f, buttonColor);
    }

    bool isMouseOverButton(double mouseX, double mouseY, float buttonX, float buttonY, float buttonWidth, float buttonHeight) {
        return mouseX >= buttonX && mouseX <= buttonX + buttonWidth && mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
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

    GLFWwindow* window = glfwCreateWindow(1600, 1200, "Tetris 3D", nullptr, nullptr);
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
    GameState state = Playing;
    Game game(4, 16, 4);
    Renderer renderer;
    Menu menu(window, state);
    HowToPlayScreen howToPlayScreen(window, state);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewportWidth / viewportHeight, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(15, 25, 15), glm::vec3(5, 10, 5), glm::vec3(0, 1, 0));

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSetWindowUserPointer(window, &game);
                glfwSetKeyCallback(window, key_callback);
        
        switch (state) {
            case MenuPrincipal:
                menu.displayMenu(); // display the menu
                break;
            case Playing:
                if(game.getIsRunning()){
                    game.update(0.016f); // Start the game
                    renderer.renderGame(game, projection, view);
                } else {
                    state = GameOver;
                }
                break;
            case HowToPlay:
                howToPlayScreen.display();
                break;
            case GameOver:
                game.start();
                // Optionally implement a game over screen if needed
                state = MenuPrincipal; // Ensure it loops back to the menu
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
