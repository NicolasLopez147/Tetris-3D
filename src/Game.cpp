#include "Grid.cpp"

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
    int nextShape;

    const int LINES_PER_LEVEL = 10;
    const float SPEED_INCREMENT = 0.1f;

    int setShape(){
        return rand()/6;
    }

    void accommodateTetromino(Tetromino& tetromino) {
        for (const auto& block :tetromino.getBlocks()) {
            Vector3D pos = block.getPosition();
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);
            int z = static_cast<int>(pos.z);
            if (x >= width) {
                tetromino.move(Vector3D(x-width, 0, 0));
            }
        }
    }

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
        nextShape = setShape();
        currentTetromino = checkPositionTetromino(Vector3D(width / 2, height - 1, depth / 2), setShape());
        nextTetromino = Tetromino(Vector3D(width + 5 , 0 , 0), nextShape);
        
    }

    Tetromino checkPositionTetromino(Vector3D pos, int shape){
        Tetromino tetromino(pos,shape);
        while(grid.checkCollision(tetromino)){
            tetromino.move(Vector3D(0,-1,0));
        }
        return tetromino;
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
        currentTetromino = Tetromino(Vector3D(width / 2, height - 1, depth / 2), nextShape);
        nextShape = setShape();
        nextTetromino = Tetromino(Vector3D(width + 5 , 0 , 0), nextShape);
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