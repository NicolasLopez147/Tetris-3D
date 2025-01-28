#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "Game.h"
class InputHandler {
public:
    // Processes user input and performs the corresponding actions on the game
    void handleInput(int key, Game& game) {
    switch (key) {
    case GLFW_KEY_S: // Move the current Tetromino down
        game.moveTetromino(glm::vec3(0, -1, 0));
        break;

    case GLFW_KEY_A: // Move the current Tetromino left
        game.moveTetromino(glm::vec3(-1, 0, 0));
        break;

    case GLFW_KEY_D: // Move the current Tetromino right
        game.moveTetromino(glm::vec3(1, 0, 0));
        break;
    
    case GLFW_KEY_Q: // Move the current Tetromino up
        game.moveTetromino(glm::vec3(0, 0, -1));
        break;

    case GLFW_KEY_E: // Rotate the Tetromino 90 degrees around the X-axis
        game.moveTetromino(glm::vec3(0, 0, 1));
        break;

    case GLFW_KEY_Z: // Rotate the Tetromino 90 degrees around the Z-axis
        game.rotateTetromino(90.0f, glm::vec3(0, 0, 1));
        break;
    case GLFW_KEY_X: // Rotate the Tetromino 90 degrees around the X-axis
        game.rotateTetromino(90.0f, glm::vec3(1, 0, 0));
        break;
    case GLFW_KEY_C: // Rotate the Tetromino 90 degrees around the Y-axis
        game.rotateTetromino(90.0f, glm::vec3(0, 1, 0));
        break;
    case GLFW_KEY_SPACE: // Restart the game
        game.moveTetrominoToProjectedPosition();
        break;
    default:
        // Optional: Handle invalid keys or no-op
        break;
    }
}

};

#endif