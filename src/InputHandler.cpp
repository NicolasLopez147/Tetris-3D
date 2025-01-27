#include "Game.cpp"

class InputHandler {
public:
    // Processes user input and performs the corresponding actions on the game
    void handleInput(int key, Game& game) {
        switch (key) {
        case 's': // Move the current Tetromino down
            game.moveTetromino(Vector3D(0, -1, 0));
            break;

        case 'a': // Move the current Tetromino left
            game.moveTetromino(Vector3D(-1, 0, 0));
            break;

        case 'd': // Move the current Tetromino right
            game.moveTetromino(Vector3D(1, 0, 0));
            break;

        case 'w': // Rotate the Tetromino 90 degrees around the Z-axis
            game.rotateTetromino(90, Vector3D(0, 0, 1));
            break;

        case 'q': // Rotate the Tetromino 90 degrees around the Y-axis
            game.rotateTetromino(90, Vector3D(0, 1, 0));
            break;

        case 'e': // Rotate the Tetromino 90 degrees around the X-axis
            game.rotateTetromino(90, Vector3D(1, 0, 0));
            break; 

        default:
            // Optional: Handle invalid keys or no-op
            break;
        }
    }
};
