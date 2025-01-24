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
    
    // Constructor: Initializes a block with a given position and color
    Block(const Vector3D& pos, const Vector3D& col) : position(pos), color(col) {}

    // Method to draw the block
    void draw(bool drawEdges = true) const {
        glPushMatrix();

        // Apply position and color for the block
        glTranslatef(position.x, position.y, position.z);
        glColor3f(color.x, color.y, color.z);

        // Draw a cube with solid faces
        glBegin(GL_QUADS);

        // Front face
        glVertex3f(-0.1f, -0.1f,  0.1f);
        glVertex3f( 0.1f, -0.1f,  0.1f);
        glVertex3f( 0.1f,  0.1f,  0.1f);
        glVertex3f(-0.1f,  0.1f,  0.1f);

        // Back face
        glVertex3f(-0.1f, -0.1f, -0.1f);
        glVertex3f(-0.1f,  0.1f, -0.1f);
        glVertex3f( 0.1f,  0.1f, -0.1f);
        glVertex3f( 0.1f, -0.1f, -0.1f);

        // Top face
        glVertex3f(-0.1f,  0.1f, -0.1f);
        glVertex3f(-0.1f,  0.1f,  0.1f);
        glVertex3f( 0.1f,  0.1f,  0.1f);
        glVertex3f( 0.1f,  0.1f, -0.1f);

        // Bottom face
        glVertex3f(-0.1f, -0.1f, -0.1f);
        glVertex3f( 0.1f, -0.1f, -0.1f);
        glVertex3f( 0.1f, -0.1f,  0.1f);
        glVertex3f(-0.1f, -0.1f,  0.1f);

        // Right face
        glVertex3f( 0.1f, -0.1f, -0.1f);
        glVertex3f( 0.1f,  0.1f, -0.1f);
        glVertex3f( 0.1f,  0.1f,  0.1f);
        glVertex3f( 0.1f, -0.1f,  0.1f);

        // Left face
        glVertex3f(-0.1f, -0.1f, -0.1f);
        glVertex3f(-0.1f, -0.1f,  0.1f);
        glVertex3f(-0.1f,  0.1f,  0.1f);
        glVertex3f(-0.1f,  0.1f, -0.1f);

        glEnd();

        // Optionally draw edges
        if (drawEdges) {
            glColor3f(0.0f, 0.0f, 0.0f); // Black edges
            glLineWidth(2.0f);
            glBegin(GL_LINES);

            // Front face edges
            glVertex3f(-0.1f, -0.1f,  0.1f); glVertex3f( 0.1f, -0.1f,  0.1f);
            glVertex3f( 0.1f, -0.1f,  0.1f); glVertex3f( 0.1f,  0.1f,  0.1f);
            glVertex3f( 0.1f,  0.1f,  0.1f); glVertex3f(-0.1f,  0.1f,  0.1f);
            glVertex3f(-0.1f,  0.1f,  0.1f); glVertex3f(-0.1f, -0.1f,  0.1f);

            // Back face edges
            glVertex3f(-0.1f, -0.1f, -0.1f); glVertex3f( 0.1f, -0.1f, -0.1f);
            glVertex3f( 0.1f, -0.1f, -0.1f); glVertex3f( 0.1f,  0.1f, -0.1f);
            glVertex3f( 0.1f,  0.1f, -0.1f); glVertex3f(-0.1f,  0.1f, -0.1f);
            glVertex3f(-0.1f,  0.1f, -0.1f); glVertex3f(-0.1f, -0.1f, -0.1f);

            glEnd();
        }

        glPopMatrix();
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
