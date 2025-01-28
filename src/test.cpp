#include "Game.h"

void test_Block() {
    Block block(glm::vec3(1, 2, 3), glm::vec3(1, 0, 0));
    assert(block.getPosition().x == 1 && block.getPosition().y == 2 && block.getPosition().z == 3);
    block.setPosition(glm::vec3(5, 5, 5));
    assert(block.getPosition().x == 5 && block.getPosition().y == 5 && block.getPosition().z == 5);
}

void test_TetrominoMovement() {
    int width = 10;
    int height = 20;
    int depth = 10;
    Tetromino t(glm::vec3(width / 2, height - 1, depth / 2),1);
    assert(!t.getBlocks().empty());
    for (const auto& block : t.getBlocks()) {
            glm::vec3 pos = block.getPosition();
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);
            int z = static_cast<int>(pos.z);
            std::cout << x <<" "<< y <<" "<<z<< std::endl;
    }
    std::cout << " ---------------------------------------------- "<< std::endl;
    t.move(glm::vec3(0, -1, 0));
    assert(!t.getBlocks().empty());
    for (const auto& block : t.getBlocks()) {
            glm::vec3 pos = block.getPosition();
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);
            int z = static_cast<int>(pos.z);
            std::cout << x <<" "<< y <<" "<<z<< std::endl;
    }

}

void test_Grid() {
    int width = 10;
    int height = 20;
    int depth = 10;
    Grid grid(width, height, depth);
    Tetromino t(glm::vec3(width / 2, height - 3, depth / 2),1);
    assert(!grid.checkCollision(t));
    while(!grid.checkCollision(t)){
        t.move(glm::vec3(-1, 0, 0));
        std::cout<< "-----------------------------------"<<std::endl;
        for (const auto& block : t.getBlocks()) {
                glm::vec3 pos = block.getPosition();
                int x = static_cast<int>(pos.x);
                int y = static_cast<int>(pos.y);
                int z = static_cast<int>(pos.z);
                std::cout << x <<" "<< y <<" "<<z<< std::endl;
        }
    }
}

void test_Game() {
    Game game(10, 20, 10);
    game.start();
    assert(game.getIsRunning() == true);

    game.update(1.0f);
    assert(game.getIsRunning() == true);
}