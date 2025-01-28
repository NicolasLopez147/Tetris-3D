#ifndef GAME_H
#define GAME_H

#include "Grid.h"
#include <random>

class Game{
    private:

        Grid grid;
        Tetromino currentTetromino;
        Tetromino nextTetromino;
        bool isRunning;
        int score;
        int level;
        int linesCleared;
        int linesClearedTotal;
        float fallSpeed;
        int WIDTH = 4;
        int HEIGHT = 16;
        int DEPTH = 4;

        int nextShape;

        const int LINES_PER_LEVEL = 10;
        const float SPEED_INCREMENT = 0.1f;
        const glm::vec3 POSITION_NEW_TETROMINO = glm::vec3(WIDTH/2, HEIGHT, DEPTH/2);
        const glm::vec3 POSITION_NEXT_TETROMINO = glm::vec3(WIDTH + 3, HEIGHT/2, 0);
        const float INITIAL_FALL_SPEED = 0.8f;

        int setShape(){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, 6);
            return dist(gen);
        }

        void checkPositionTetromino(Tetromino& tetromino){
            for (const auto& block : tetromino.getBlocks()){
                glm::vec3 blockPos = block.getPosition();
                if (blockPos.y >= HEIGHT){
                    tetromino.move(glm::vec3(0, -(blockPos.y - HEIGHT + 1), 0));
                }else if(blockPos.y < 0){
                    tetromino.move(glm::vec3(0, -blockPos.y, 0));
                }
                if (blockPos.x >= WIDTH){
                    tetromino.move(glm::vec3(-(blockPos.x - WIDTH + 1),0,0));
                }else if (blockPos.x < 0){
                    tetromino.move(glm::vec3(-blockPos.x,0,0));
                }
                if (blockPos.z >= DEPTH){
                    tetromino.move(glm::vec3(0,0,-(blockPos.z - DEPTH + 1)));
                } else if (blockPos.z < 0){
                    tetromino.move(glm::vec3(0,0,-blockPos.z));
                }
            }
        }

        Tetromino calculateProjection(const Tetromino& tetromino) const {
            Tetromino projectedTetromino = tetromino;

            while (!grid.checkCollision(projectedTetromino)){
                projectedTetromino.move(glm::vec3(0, -1, 0));
            }

            projectedTetromino.move(glm::vec3(0, 1, 0));

            return projectedTetromino;
        }

        bool checkGameOver(Tetromino currentTetromino) const{
            return grid.checkCollision(currentTetromino);
        }

    public:
        Game(int width, int height, int depth): grid(width, height, depth), WIDTH(width) , HEIGHT(height), DEPTH(depth){
            start();
        }

        void start(){
            isRunning = true;
            score = 0;
            level = 0;
            linesCleared = 0;
            linesClearedTotal = 0;
            fallSpeed = INITIAL_FALL_SPEED;
            grid = Grid(WIDTH, HEIGHT, DEPTH);
            nextShape = setShape();
            currentTetromino = Tetromino(POSITION_NEW_TETROMINO, setShape());
            checkPositionTetromino(currentTetromino);
            nextTetromino = Tetromino(POSITION_NEXT_TETROMINO, nextShape);
        }


        void update(float deltaTime) {
            static float accumulatedTime = 0.0f;
            accumulatedTime += deltaTime;

            fallSpeed = std::max( INITIAL_FALL_SPEED - ((INITIAL_FALL_SPEED / 15) * level), 0.01f);

            if (accumulatedTime >= fallSpeed) {
                // Move the current Tetromino down
                currentTetromino.move(glm::vec3(0, -1, 0));
                
                // Place the Tetromino and clear lines if it collides
                if (grid.checkCollision(currentTetromino)) {
                    currentTetromino.move(glm::vec3(0, 1, 0)); // Undo the move
                    grid.placeTetromino(currentTetromino);
                    linesCleared += grid.clearLines();
                    if (linesCleared == 1){
                        score += 40 * (level + 1);
                    } else if (linesCleared == 2){
                        score += 100 * (level + 1);
                    } else if (linesCleared == 3){
                        score += 300 * (level + 1);
                    } else if (linesCleared == 4){
                        score += 1200 * (level + 1);
                    }
                    linesClearedTotal += linesCleared;
                    linesCleared = 0;
                    level = linesClearedTotal/LINES_PER_LEVEL; ;

                    // Set up the next Tetromino
                    currentTetromino = Tetromino(POSITION_NEW_TETROMINO, nextShape,nextTetromino.getColor());
                    checkPositionTetromino(currentTetromino);
                    nextShape = setShape();
                    nextTetromino = Tetromino(POSITION_NEXT_TETROMINO, nextShape);

                    // Check if the game is over
                    isRunning = !checkGameOver(currentTetromino);
                }

                accumulatedTime = 0.0f;
            }
        }

        Tetromino getProjectedTetromino(const Tetromino& tetromino) const {
            return calculateProjection(tetromino);
        }

        void moveTetromino(const glm::vec3& direction){
            currentTetromino.move(glm::vec3(direction.x, direction.y, direction.z));
            if (grid.checkCollision(currentTetromino)){
                currentTetromino.move(glm::vec3(-direction.x, -direction.y, -direction.z));
            }
        }

        void rotateTetromino(float angle, const glm::vec3& axis){
            currentTetromino.rotate(angle, glm::vec3(axis.x, axis.y, axis.z));
            checkPositionTetromino(currentTetromino);
            if (grid.checkCollision(currentTetromino)){
                currentTetromino.rotate(-angle, glm::vec3(axis.x, axis.y, axis.z));
            }
        }

        void moveTetrominoToProjectedPosition(){
            currentTetromino = calculateProjection(currentTetromino);
        }

        int getScore() const{
            return score;
        }

        int getTotalLinesCleared() const{
            return linesClearedTotal;
        }

        bool getIsRunning() const{
            return isRunning;
        }

        Grid getGrid() const{
            return grid;
        }

        Tetromino getCurrentTetromino() const{
            return currentTetromino;
        }

        Tetromino getNextTetromino() const{
            return nextTetromino;
        }

        int getLevel() const{
            return level;
        }

        ~Game(){
            grid.cleanUp();
        }
};
#endif