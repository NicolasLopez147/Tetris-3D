#include <iostream>
#include "Vector.cpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Block {
private:
    // Attribute to store the block's position in 3D space
    Vector3D position;

    // Attribute to store the block's color, represented as an RGB vector
    Vector3D color;

public:

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
    
    // Constructor: Initializes a block with a given position and color
    Block(const Vector3D& pos, const Vector3D& col) : position(pos), color(col) {}

    // Method to draw the block
    void draw()  {
        // Set the color of the block
        glColor3f(color.x, color.y, color.z);

        // Draw the block as a cube
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < cubeIndices.size(); i++) {
            glVertex3f(cubeVertices[cubeIndices[i] * 3], cubeVertices[cubeIndices[i] * 3 + 1], cubeVertices[cubeIndices[i] * 3 + 2]);
        }
        glEnd();
    }

    // Method to set a new position for the block
    void setPosition(const Vector3D& newPos) { position = newPos; }

    // Method to retrieve the current position of the block
    Vector3D getPosition() const { return position; }

    // Method to set a new color for the block
    void setColor(const Vector3D& newColor) { color = newColor; }

    // Method to retrieve the current color of the block
    Vector3D getColor() const { return color; }
};
