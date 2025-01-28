#ifndef TETROMINO_H
#define TETROMINO_H

#include "Block.h"

class Tetromino {
private:
    // A collection of blocks that make up the Tetromino
    std::vector<Block> blocks;

    // Color of the Tetromino, applied to all its blocks
    glm::vec3 color;

    glm::vec3 center;

    // Rotation state of the Tetromino
    glm::mat4 rotation;


    // Maximum value for random color generation (normalized to 1.0f)
    const int rand_max = 255;

    // Generates a random color for the Tetromino
    glm::vec3 randomColor() {
        float r = static_cast<float>(rand()) / RAND_MAX; // Generate red component
        float g = static_cast<float>(rand()) / RAND_MAX; // Generate green component
        float b = static_cast<float>(rand()) / RAND_MAX; // Generate blue component
        return glm::vec3(r, g, b);
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

    // Sets the Tetromino's shape by adding blocks in a predefined configuration
        void setShape(int shape) {
        switch (shape) {
        case 0: // I-shape
            addBlock(Block(glm::vec3(0, 0, 0), color));
            addBlock(Block(glm::vec3(1, 0, 0), color));
            addBlock(Block(glm::vec3(2, 0, 0), color));
            addBlock(Block(glm::vec3(3, 0, 0), color));
            break;
        case 1: // J-shape
            addBlock(Block(glm::vec3(0, 0, 0), color));
            addBlock(Block(glm::vec3(1, 0, 0), color));
            addBlock(Block(glm::vec3(1, 1, 0), color));
            addBlock(Block(glm::vec3(1, 2, 0), color));
            break;
        case 2: // L-shape
            addBlock(Block(glm::vec3(0, 0, 0), color));
            addBlock(Block(glm::vec3(1, 0, 0), color));
            addBlock(Block(glm::vec3(0, 1, 0), color));
            addBlock(Block(glm::vec3(0, 2, 0), color));
            break;
        case 3: // O-shape
            addBlock(Block(glm::vec3(0, 0, 0), color));
            addBlock(Block(glm::vec3(1, 0, 0), color));
            addBlock(Block(glm::vec3(0, 1, 0), color));
            addBlock(Block(glm::vec3(1, 1, 0), color));
            break;
        case 4: // S-shape
            addBlock(Block(glm::vec3(0, 0, 0), color));
            addBlock(Block(glm::vec3(0, 1, 0), color));
            addBlock(Block(glm::vec3(1, 1, 0), color));
            addBlock(Block(glm::vec3(2, 1, 0), color));
            break;
        case 5: // T-shape
            addBlock(Block(glm::vec3(0, 0, 0), color));
            addBlock(Block(glm::vec3(1, 0, 0), color));
            addBlock(Block(glm::vec3(2, 0, 0), color));
            addBlock(Block(glm::vec3(1, 1, 0), color));
            break;
        case 6: // Z-shape
            addBlock(Block(glm::vec3(0, 1, 0), color));
            addBlock(Block(glm::vec3(1, 1, 0), color));
            addBlock(Block(glm::vec3(1, 0, 0), color));
            addBlock(Block(glm::vec3(2, 0, 0), color));
            break;
        default:
            break;
        }
    }

    // Adds a block to the Tetromino
    void addBlock(const Block& block) {
        blocks.push_back(block);
    }

public:
    Tetromino() {}
    // Constructor: Initializes the Tetromino at a position with a specific shape
    Tetromino(const glm::vec3& pos, int shape) : color(randomColor()) , rotation(glm::mat4(1.0f)) {
        setShape(shape);
        calculateCenter();
        move(glm::vec3 (pos.x, pos.y, pos.z)); // Adjust blocks to the initial position
    }

    Tetromino(const glm::vec3& pos, int shape, const glm::vec3& col) : color(col) , rotation(glm::mat4(1.0f)) {
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

    // Moves the Tetromino by a given vector
    void move(glm::vec3 direction) {
        for (auto& block : blocks) {
            block.setPosition(block.getPosition() + direction);
        }
    }

    glm::vec3 getColor(){
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

#endif