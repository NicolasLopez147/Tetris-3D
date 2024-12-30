#include <iostream>
#include <cmath>
#include <vector>

class Vector3D {
public:
    float x, y, z;

    Vector3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    Vector3D operator+(const Vector3D& other) const {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }
    Vector3D operator-(const Vector3D& other) const {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }
    Vector3D operator/(float scalar) const {
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }
};

class Block {
protected:
    Vector3D position;
    Vector3D color;
public:
    Block(const Vector3D& pos, const Vector3D& col) : position(pos), color(col) {}
    virtual void draw() const {
        std::cout << "Drawing block at position (" << position.x << ", " << position.y << ", " << position.z << ") with color (" << color.x << ", " << color.y << ", " << color.z << ")" << std::endl;
    }
    Vector3D getPosition() const { return position; }
    void setPosition(const Vector3D& newPos) { position = newPos; }
};

class Tetromino : public Block {
private:
    std::vector<Block> blocks;
    Vector3D rotationAxis;
    Vector3D center;

    void calculateCenter() {
        if (blocks.empty()) return;
        Vector3D sum(0, 0, 0);
        for (const auto& block : blocks) {
            sum = sum + block.getPosition();
        }
        center = sum / blocks.size();
    }

public:
    Tetromino() : Block(Vector3D(), Vector3D()), center(Vector3D()) {}
    Tetromino(const Vector3D& pos, const Vector3D& col) : Block(pos, col), center(pos) {}

    void draw() const override {
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

class Grid {
private:
    int width, height, depth;
    std::vector<std::vector<std::vector<bool>>> cells;
    std::vector<int> lineCounters;
public:
    Grid(int w, int h, int d) : width(w), height(h), depth(d), cells(w, std::vector<std::vector<bool>>(h, std::vector<bool>(d, false))), lineCounters(h, 0) {}

    bool checkCollision(const Tetromino& tetromino) const {
        for (const auto& block : tetromino.getBlocks()) {
            Vector3D pos = block.getPosition();
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);
            int z = static_cast<int>(pos.z);

            if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
                return true;
            }
            if (cells[x][y][z]) {
                return true;
            }
        }
        return false;
    }

    void placeTetromino(const Tetromino& tetromino) {
        for (const auto& block : tetromino.getBlocks()) {
            Vector3D pos = block.getPosition();
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);
            int z = static_cast<int>(pos.z);
            if (!cells[x][y][z]) {
                cells[x][y][z] = true;
                lineCounters[y]++;
            }
        }
    }

    void clearLines() {
        for (int y = 0; y < height; ++y) {
            if (lineCounters[y] == width * depth) {
                for (int z = 0; z < depth; ++z) {
                    for (int x = 0; x < width; ++x) {
                        cells[x][y][z] = false;
                    }
                }
                for (int ny = y; ny < height - 1; ++ny) {
                    for (int z = 0; z < depth; ++z) {
                        for (int x = 0; x < width; ++x) {
                            cells[x][ny][z] = cells[x][ny + 1][z];
                        }
                    }
                    lineCounters[ny] = lineCounters[ny + 1];
                }
                lineCounters[height - 1] = 0;
            }
        }
    }

    void draw() const;
};

class Game {
private:
    Grid grid;
    Tetromino currentTetromino;
    Tetromino nextTetromino;
    bool isRunning;
    int score;
    int level;
    int linesCleared;
    float fallSpeed;
    int width, height, depth;

    const int LINES_PER_LEVEL = 10;
    const float SPEED_INCREMENT = 0.1f;

public:
    Game(int gridWidth, int gridHeight, int gridDepth): width(gridWidth), height(gridHeight), depth(gridDepth),grid(gridWidth, gridHeight, gridDepth), isRunning(true), score(0), level(1), linesCleared(0), fallSpeed(1.0f) {}

    bool getIsRunning() const { return isRunning; }
    void start() {
        isRunning = true;
        score = 0;
        level = 1;
        linesCleared = 0;
        fallSpeed = 1.0f;
        grid = Grid(width, height, depth);
        currentTetromino = Tetromino(Vector3D(width / 2, height - 1, depth / 2), Vector3D(1, 0, 0));
        nextTetromino = Tetromino(Vector3D(width / 2, height - 1, depth / 2), Vector3D(0, 1, 0));
    }

    bool checkGameOver(Tetromino currentTetromino) const{
        return grid.checkCollision(currentTetromino);
    }
    void update(float deltaTime){
      static float accumulatedTime = 0.0f;
      accumulatedTime += deltaTime;
      if (accumulatedTime >= fallSpeed) {
        currentTetromino.move(Vector3D(0, -1, 0));
        grid.placeTetromino(currentTetromino);
        grid.clearLines();
        linesCleared += 1;

        if(linesCleared % LINES_PER_LEVEL == 0){
          level += 1;
          linesCleared = 0;
          fallSpeed = std::max(fallSpeed - SPEED_INCREMENT, 0.2f);
        }
        currentTetromino = nextTetromino;
        nextTetromino = Tetromino(Vector3D(width / 2, height - 1, depth / 2), Vector3D(0, 1, 0));
        isRunning = !checkGameOver(currentTetromino);
        accumulatedTime = 0.0f;
      }
    }

    void moveTetromino(const Vector3D& direction){
      currentTetromino.move(direction);
      if(grid.checkCollision(currentTetromino)){
        currentTetromino.move((-direction.x,-direction.y,-direction.z));
      }
    }
    void rotateTetromino(float angle, const Vector3D& axis){
      currentTetromino.rotate(angle,axis);
      if(grid.checkCollision(currentTetromino)){
        currentTetromino.rotate(-angle,axis);
      }
    }
    void draw() const;
};

class InputHandler {
  public:
    void handleInput(int key, Game& game){
      switch(key){
        case 's':
          game.moveTetromino(Vector3D(0,-1,0));
          break;
        case 'a':
          game.moveTetromino(Vector3D(-1,0,0));
          break;
        case 'd':
          game.moveTetromino(Vector3D(1,0,0));
          break;
        case 'w':
          game.rotateTetromino(90,Vector3D(0,0,1));
          break;
        case 'q':
          game.rotateTetromino(90,Vector3D(0,1,0));
          break;
        case 'e':
          game.rotateTetromino(90,Vector3D(1,0,0));
          break;
      }
    }
};

int main() {
    Game game(10, 20, 10);
    game.start();
    InputHandler inputHandler;
    while (game.getIsRunning()) {
        std::cout<<"Game is running"<<std::endl;
        game.update(0.1f);
        // game.draw();
        char key;
        std::cin >> key;
        inputHandler.handleInput(key,game);
    }

    return 0;
}
