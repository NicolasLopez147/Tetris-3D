#include "Block.cpp"
#include <vector>
#include <math.h>

class Tetromino {
private:
    // A collection of blocks that make up the Tetromino
    std::vector<Block> blocks;

    // Axis of rotation for the Tetromino
    Vector3D rotationAxis;

    // Center of the Tetromino, used for rotation and positioning
    Vector3D center;

    // Color of the Tetromino, applied to all its blocks
    Vector3D color;

    // Maximum value for random color generation (normalized to 1.0f)
    const int rand_max = 255;

    // Recalculates the center of the Tetromino based on its blocks
    void calculateCenter() {
        if (blocks.empty()) return;
        Vector3D sum(0, 0, 0);
        for (const auto& block : blocks) {
            sum = sum + block.getPosition();
        }
        center = sum / blocks.size();
    }

public:
    // Default constructor
    Tetromino() {}

    // Constructor: Initializes the Tetromino at a position with a specific shape
    Tetromino(const Vector3D& pos, int shape) : center(pos), color(randomColor()) {
        setShape(shape);
        move(pos); // Adjust blocks to the initial position
    }

    // Overloaded assignment operator
    Tetromino& operator=(const Tetromino& other) {
        if (this != &other) {
            blocks = other.blocks;
            rotationAxis = other.rotationAxis;
            center = other.center;
            color = other.color;
        }
        return *this;
    }

    // Adds a block to the Tetromino
    void addBlock(const Block& block) {
        blocks.push_back(block);
    }

    // Generates a random color for the Tetromino
    Vector3D randomColor() {
        return Vector3D(static_cast<float>(rand()) / rand_max,
                        static_cast<float>(rand()) / rand_max,
                        static_cast<float>(rand()) / rand_max);
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

    // Draws the Tetromino by drawing each block
    void draw() const {
        for (const auto& block : blocks) {
            block.draw();
        }
    }

    // Rotates the Tetromino around the given axis by a specified angle
    void rotate(float angle, const Vector3D& axis) {
        calculateCenter(); // Ensure center is accurate
        float radians = angle * 3.14159265f / 180.0f;

        for (auto& block : blocks) {
            Vector3D relativePos = block.getPosition() - center;

            if (axis.x != 0) { // Rotate around X-axis
                float newY = relativePos.y * cos(radians) - relativePos.z * sin(radians);
                float newZ = relativePos.y * sin(radians) + relativePos.z * cos(radians);
                block.setPosition(center + Vector3D(relativePos.x, newY, newZ));
            }
            if (axis.y != 0) { // Rotate around Y-axis
                float newX = relativePos.x * cos(radians) + relativePos.z * sin(radians);
                float newZ = -relativePos.x * sin(radians) + relativePos.z * cos(radians);
                block.setPosition(center + Vector3D(newX, relativePos.y, newZ));
            }
            if (axis.z != 0) { // Rotate around Z-axis
                float newX = relativePos.x * cos(radians) - relativePos.y * sin(radians);
                float newY = relativePos.x * sin(radians) + relativePos.y * cos(radians);
                block.setPosition(center + Vector3D(newX, newY, relativePos.z));
            }
        }
    }

    // Moves the Tetromino in the given direction
    void move(const Vector3D& direction) {
        center = center + direction;
        for (auto& block : blocks) {
            block.setPosition(block.getPosition() + direction);
        }
    }

    // Returns a constant reference to the blocks in the Tetromino
    const std::vector<Block>& getBlocks() const { return blocks; }
};
