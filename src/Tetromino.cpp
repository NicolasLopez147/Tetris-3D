#include "Block.cpp"
#include <vector>
#include <math.h>
class Tetromino  {
private:
    std::vector<Block> blocks;
    Vector3D rotationAxis;
    Vector3D center;
    Vector3D color;
    const int rand_max = 255;

    void calculateCenter() {
        if (blocks.empty()) return;
        Vector3D sum(0, 0, 0);
        for (const auto& block : blocks) {
            sum = sum + block.getPosition();
        }
        center = sum / blocks.size();
    }


public:
    Tetromino() {}
    Tetromino(const Vector3D& pos, int shape) : center(pos) ,color(randomColor()){
        setShape(shape);
        move(pos);
    }

    Tetromino& operator=(const Tetromino& other) {
        if (this != &other) {
            blocks = other.blocks;
            rotationAxis = other.rotationAxis;
            center = other.center;
            color = other.color;
        }
        return *this;
    }
    
    void addBlock(const Block& block) {
        blocks.push_back(block);
    }
    Vector3D randomColor() {
        return Vector3D(static_cast<float>(rand()) / rand_max, static_cast<float>(rand()) / rand_max, static_cast<float>(rand()) / rand_max);
    }
    void setShape(int shape) {
        switch (shape)
        {
        case 0: // I
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(2, 0, 0), color));
            addBlock(Block(Vector3D(3, 0, 0), color));
            break;
        case 1: // J
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(1, 1, 0), color));
            addBlock(Block(Vector3D(1, 2, 0), color));
            break;
        case 2: // L
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(0, 1, 0), color));
            addBlock(Block(Vector3D(0, 2, 0), color));
            break;
        case 3: // O
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(0, 1, 0), color));
            addBlock(Block(Vector3D(1, 1, 0), color));
            break;
        case 4: // S
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(0, 1, 0), color));
            addBlock(Block(Vector3D(1, 1, 0), color));
            addBlock(Block(Vector3D(2, 1, 0), color));
            break;
        case 5: // T
            addBlock(Block(Vector3D(0, 0, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(2, 0, 0), color));
            addBlock(Block(Vector3D(1, 1, 0), color));
            break;
        case 6: // Z
            addBlock(Block(Vector3D(0, 1, 0), color));
            addBlock(Block(Vector3D(1, 1, 0), color));
            addBlock(Block(Vector3D(1, 0, 0), color));
            addBlock(Block(Vector3D(2, 0, 0), color));
            break;
        default:
            break;
        }
    }

    void draw() const {
        for (const auto& block : blocks) {
            block.draw();
        }
    }

    void rotate(float angle, const Vector3D& axis) {
        calculateCenter();
        float radians = angle * 3.14159265f / 180.0f;
        for (auto& block : blocks) {
            Vector3D relativePos = block.getPosition() - center;
            if (axis.x != 0) {
                float newY = relativePos.y * cos(radians) - relativePos.z * sin(radians);
                float newZ = relativePos.y * sin(radians) + relativePos.z * cos(radians);
                block.setPosition(center + Vector3D(relativePos.x, newY, newZ));
            }
            if (axis.y != 0) {
                float newX = relativePos.x * cos(radians) + relativePos.z * sin(radians);
                float newZ = -relativePos.x * sin(radians) + relativePos.z * cos(radians);
                block.setPosition(center + Vector3D(newX, relativePos.y, newZ));
            }
            if (axis.z != 0) {
                float newX = relativePos.x * cos(radians) - relativePos.y * sin(radians);
                float newY = relativePos.x * sin(radians) + relativePos.y * cos(radians);
                block.setPosition(center + Vector3D(newX, newY, relativePos.z));
            }
        }
    }

    void move(const Vector3D& direction) {
        center = center + direction;
        for (auto& block : blocks) {
            block.setPosition(block.getPosition() + direction);
        }
    }

    const std::vector<Block>& getBlocks() const { return blocks; }
};