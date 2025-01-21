#include "Grid.cpp"
#include <random>

class Game {
private:
    // The grid where the game takes place
    Grid grid;

    // The current Tetromino that is falling
    Tetromino currentTetromino;

    // The next Tetromino to appear
    Tetromino nextTetromino;

    // Game state flag: true if the game is running
    bool isRunning;

    // Player's score
    int score;

    // Current game level
    int level;

    // Number of lines cleared so far
    int linesCleared;

    // Current fall speed of the Tetromino
    float fallSpeed;

    // Dimensions of the grid
    int width, height, depth;

    // Shape identifier for the next Tetromino
    int nextShape;

    // Constants for level progression
    const int LINES_PER_LEVEL = 10;     // Lines needed to level up
    const float SPEED_INCREMENT = 0.1f; // Speed increase per level
    const Vector3D POSITION_NEW_TETROMINO = Vector3D(0, height , 0);
    const Vector3D POSITION_NEXT_TETROMINO = Vector3D(width + 5, 0 , 0);

    // Generates a random shape ID (0-6 for Tetromino shapes)
    int setShape() {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> dist(0, 6);
      return dist(gen);
    }

    // Ensures a Tetromino is placed at a valid position on the grid
    Tetromino checkPositionTetromino(Vector3D pos, int shape) {
        Tetromino tetromino(pos, shape);
        for (const auto& block : tetromino.getBlocks()) {
            Vector3D blockPos = block.getPosition();
            if (blockPos.y >= height) {
                tetromino.move(Vector3D(0, -(blockPos.y - height + 1), 0));
            }
        }
        return tetromino;
    }

public:
    // Constructor: Initializes the game with the given grid dimensions
    Game(int gridWidth, int gridHeight, int gridDepth)
        : width(gridWidth), height(gridHeight), depth(gridDepth),
          grid(gridWidth, gridHeight, gridDepth),
          isRunning(true), score(0), level(1), linesCleared(0), fallSpeed(1.0f) {}

    // Returns whether the game is currently running
    bool getIsRunning() const { return isRunning; }

    // Starts or resets the game
    void start() {
        isRunning = true;
        score = 0;
        level = 1;
        linesCleared = 0;
        fallSpeed = 1.0f;
        grid = Grid(width, height, depth);
        nextShape = setShape();
        currentTetromino = checkPositionTetromino(POSITION_NEW_TETROMINO, setShape());
        nextTetromino = Tetromino(POSITION_NEXT_TETROMINO, nextShape);
    }

    // Checks if the game is over based on the current Tetromino's position
    bool checkGameOver(Tetromino currentTetromino) const {
        return grid.checkCollision(currentTetromino);
    }

    // Updates the game state based on the time elapsed
    void update(float deltaTime) {
      grid.printGrid();
        static float accumulatedTime = 0.0f;
        accumulatedTime += deltaTime;

        if (accumulatedTime >= fallSpeed) {
            // Move the current Tetromino down
            currentTetromino.move(Vector3D(0, -1, 0));

            // Place the Tetromino and clear lines if it collides
            if (grid.checkCollision(currentTetromino)) {
                currentTetromino.move(Vector3D(0, 1, 0)); // Undo the move
                grid.placeTetromino(currentTetromino);
                linesCleared += grid.clearLines();

                // Level up if enough lines are cleared
                if (linesCleared % LINES_PER_LEVEL == 0) {
                    level++;
                    linesCleared = 0;
                    fallSpeed = std::max(fallSpeed - SPEED_INCREMENT, 0.2f); // Limit minimum speed
                }

                // Set up the next Tetromino
                currentTetromino = checkPositionTetromino(POSITION_NEW_TETROMINO, nextShape);
                nextShape = setShape();
                nextTetromino = Tetromino(POSITION_NEXT_TETROMINO, nextShape);

                // Check if the game is over
                isRunning = !checkGameOver(currentTetromino);
            }

            accumulatedTime = 0.0f;
        }
    }

    // Moves the current Tetromino in a given direction
    void moveTetromino(const Vector3D& direction) {
        currentTetromino.move(direction);
        if (grid.checkCollision(currentTetromino)) {
            currentTetromino.move(Vector3D(-direction.x, -direction.y, -direction.z)); // Undo the move
        }
    }

    // Rotates the current Tetromino around a specified axis
    void rotateTetromino(float angle, const Vector3D& axis) {
        currentTetromino.rotate(angle, axis);
        if (grid.checkCollision(currentTetromino)) {
            currentTetromino.rotate(-angle, axis); // Undo the rotation
        }
    }

    int getScore() const { return score; }

    int getLinesCleared() const { return linesCleared; }
    // Draws the game elements (grid, Tetrominos, etc.)
    void draw() const;
};
