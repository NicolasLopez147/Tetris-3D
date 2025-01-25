#include <iostream>
#include "Vector.cpp"

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
    // Outputs the block's position and color to the console
    void draw() const {
        std::cout << "Drawing block at position (" << position.x << ", " << position.y << ", " << position.z 
                  << ") with color (" << color.x << ", " << color.y << ", " << color.z << ")" << std::endl;
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
