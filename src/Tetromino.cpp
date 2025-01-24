#include "Block.cpp"
#include <vector>
#include <math.h>
#include <GL/glew.h> // OpenGL Extension Wrangler Library for managing OpenGL extensions
#include <GLFW/glfw3.h> // Library for creating windows and handling input
#include <glm/glm.hpp> // GLM library for vector and matrix operations
#include <glm/gtc/matrix_transform.hpp> // GLM utilities for transformations (e.g., translate, rotate)
#include <glm/gtc/type_ptr.hpp> // GLM utility to convert matrices to pointer types for OpenGL
#include <iostream> // Standard input/output stream library
#include <cstdlib> // Standard library for utility functions
#include <cmath> // Math library for calculations

class Tetromino {
private:
    // A collection of blocks that make up the Tetromino
    std::vector<Block> blocks;

    // Color of the Tetromino, applied to all its blocks
    Vector3D color;

    // Rotation state of the Tetromino
    glm::mat4 rotation;

    // Global position of the Tetromino
    Vector3D globalPosition;

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

public:
    // Default constructor
    Tetromino() {}

    // Constructor: Initializes the Tetromino with a specific shape and position
    Tetromino(const Vector3D& initialPosition, int shape = 0)
        : color(randomColor()), rotation(glm::mat4(1.0f)), globalPosition(initialPosition) {
        setShape(shape);
    }

    // Constructor: Initializes the Tetromino with a specific shape
    Tetromino(int shape) : color(randomColor()), rotation(glm::mat4(1.0f)), globalPosition(0.0f, 0.0f, 0.0f) {
        static bool initialized = false; // Ensures random seed is initialized once
        if (!initialized) {
            srand(static_cast<unsigned>(time(nullptr))); // Initialize random seed
            initialized = true;
        }
        setShape(shape); // Set the initial shape of the Tetromino
    }

    // Overloaded assignment operator
    Tetromino& operator=(const Tetromino& other) {
        if (this != &other) {
            blocks = other.blocks;
            color = other.color;
            rotation = other.rotation;
            globalPosition = other.globalPosition;
        }
        return *this;
    }

    // Adds a block to the Tetromino
    void addBlock(const Block& block) {
        blocks.push_back(block);
    }

    // Sets the Tetromino's shape by adding blocks in a predefined configuration
    void setShape(int shape) {
        blocks.clear(); // Clear any existing blocks
        switch (shape) {
        case 0: // I-shape
            addBlock(Block(Vector3D(0.0f, 0, 0), color));
            addBlock(Block(Vector3D(0.2f, 0, 0), color));
            addBlock(Block(Vector3D(0.4f, 0, 0), color));
            addBlock(Block(Vector3D(0.6f, 0, 0), color));
            break;
        case 1: // J-shape
            addBlock(Block(Vector3D(0.0f, 0.0f, 0.0f), color));
            addBlock(Block(Vector3D(0.0f, 0.2f, 0.0f), color));
            addBlock(Block(Vector3D(0.0f, 0.4f, 0.0f), color));
            addBlock(Block(Vector3D(-0.2f, 0.0f, 0.0f), color));
            break;
        case 2: // L-shape
            addBlock(Block(Vector3D(0.0f, 0.0f, 0.0f), color));
            addBlock(Block(Vector3D(0.0f, 0.2f, 0.0f), color));
            addBlock(Block(Vector3D(0.0f, 0.4f, 0.0f), color));
            addBlock(Block(Vector3D(0.2f, 0.0f, 0.0f), color));
            break;
        case 3: // O-shape
            addBlock(Block(Vector3D(0.0f, 0.0f, 0.0f), color));
            addBlock(Block(Vector3D(0.2f, 0.0f, 0.0f), color));
            addBlock(Block(Vector3D(0.0f, 0.2f, 0.0f), color));
            addBlock(Block(Vector3D(0.2f, 0.2f, 0.0f), color));
            break;
        case 4: // S-shape
            addBlock(Block(Vector3D(0.0f, 0.0f, 0.0f), color));
            addBlock(Block(Vector3D(0.2f, 0.0f, 0.0f), color));
            addBlock(Block(Vector3D(0.2f, 0.2f, 0.0f), color));
            addBlock(Block(Vector3D(0.4f, 0.2f, 0.0f), color));
            break;
        case 5: // T-shape
            addBlock(Block(Vector3D(0.0f, 0.0f, 0.0f), color));
            addBlock(Block(Vector3D(0.2f, 0.0f, 0.0f), color));
            addBlock(Block(Vector3D(0.4f, 0.0f, 0.0f), color));
            addBlock(Block(Vector3D(0.2f, 0.2f, 0.0f), color));
            break;
        case 6: // Z-shape
            addBlock(Block(Vector3D(0.0f, 0.2f, 0.0f), color));
            addBlock(Block(Vector3D(0.2f, 0.2f, 0.0f), color));
            addBlock(Block(Vector3D(0.2f, 0.0f, 0.0f), color));
            addBlock(Block(Vector3D(0.4f, 0.0f, 0.0f), color));
            break;
        default:
            break;
        }
    }

    // Moves the Tetromino by a given vector
    void move(const Vector3D& direction) {
        globalPosition = globalPosition + direction;
    }

    // Rotates the Tetromino around a specified axis by a given angle
    void rotate(float angle, const glm::vec3& axis) {
        rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis) * rotation;
    }

    // Draws the Tetromino by drawing each block
    void draw(bool drawEdges = true) const {
        glPushMatrix();

        // Apply global position
        glTranslatef(globalPosition.x, globalPosition.y, globalPosition.z);

        // Apply rotation
        glMultMatrixf(glm::value_ptr(rotation));

        for (const auto& block : blocks) {
            block.draw(drawEdges); // Draw each block
        }

        glPopMatrix();
    }

    // Returns a constant reference to the blocks in the Tetromino
    const std::vector<Block>& getBlocks() const { return blocks; }
};
