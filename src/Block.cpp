#include <iostream>
#include "Vector.cpp"
class Block {
private:
    Vector3D position;
    Vector3D color;
public:
    Block(const Vector3D& pos, const Vector3D& col) : position(pos), color(col) {}
    void draw() const {
        std::cout << "Drawing block at position (" << position.x << ", " << position.y << ", " << position.z << ") with color (" << color.x << ", " << color.y << ", " << color.z << ")" << std::endl;
    }
    void setPosition(const Vector3D& newPos) { position = newPos; }
    Vector3D getPosition() const { return position; }

    void setColor(const Vector3D& newColor) { color = newColor; }
    Vector3D getColor() const { return color; }
};