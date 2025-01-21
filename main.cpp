#include <iostream>
#include "src\Game.cpp"
#include <assert.h>

void test_Vector3D() {
    Vector3D v1(1, 2, 3);
    Vector3D v2(2, 3, 4);

    Vector3D sum = v1 + v2;
    assert(sum.x == 3 && sum.y == 5 && sum.z == 7);

    Vector3D diff = v1 - v2;
    assert(diff.x == -1 && diff.y == -1 && diff.z == -1);

    Vector3D div = v1 / 2.0f;
    assert(div.x == 0.5f && div.y == 1.0f && div.z == 1.5f);
}

void test_Block() {
    Block block(Vector3D(1, 2, 3), Vector3D(1, 0, 0));
    assert(block.getPosition().x == 1 && block.getPosition().y == 2 && block.getPosition().z == 3);
    block.setPosition(Vector3D(5, 5, 5));
    assert(block.getPosition().x == 5 && block.getPosition().y == 5 && block.getPosition().z == 5);
}

void test_TetrominoMovement() {
    int width = 10;
    int height = 20;
    int depth = 10;
    Tetromino t(Vector3D(width / 2, height - 1, depth / 2),1);
    assert(!t.getBlocks().empty());
    // for (const auto& block : t.getBlocks()) {
    //         Vector3D pos = block.getPosition();
    //         int x = static_cast<int>(pos.x);
    //         int y = static_cast<int>(pos.y);
    //         int z = static_cast<int>(pos.z);
    //         std::cout << x <<" "<< y <<" "<<z<< std::endl;
    // }
    // std::cout << " ---------------------------------------------- "<< std::endl;
    t.move(Vector3D(0, -1, 0));
    assert(!t.getBlocks().empty());
    // for (const auto& block : t.getBlocks()) {
    //         Vector3D pos = block.getPosition();
    //         int x = static_cast<int>(pos.x);
    //         int y = static_cast<int>(pos.y);
    //         int z = static_cast<int>(pos.z);
    //         std::cout << x <<" "<< y <<" "<<z<< std::endl;
    // }

}

void test_Grid() {
    int width = 10;
    int height = 20;
    int depth = 10;
    Grid grid(width, height, depth);
    Tetromino t(Vector3D(width / 2, height - 3, depth / 2),1);
    assert(!grid.checkCollision(t));
    while(!grid.checkCollision(t)){
        t.move(Vector3D(-1, 0, 0));
        // std::cout<< "-----------------------------------"<<std::endl;
        // for (const auto& block : t.getBlocks()) {
        //         Vector3D pos = block.getPosition();
        //         int x = static_cast<int>(pos.x);
        //         int y = static_cast<int>(pos.y);
        //         int z = static_cast<int>(pos.z);
        //         std::cout << x <<" "<< y <<" "<<z<< std::endl;
        // }
    }
}

void test_Game() {
    
    Game game(10, 20, 10);
    
    game.start();

    
    // assert(game.getIsRunning() == true);

    // game.update(1.0f);
    
    // assert(game.getIsRunning() == true);
    // std::cout << "Game started" << std::endl;
}

int main() {

    // Game game(10, 20, 10);
    // game.start();
    // InputHandler inputHandler;
    // while (game.getIsRunning()) {
    //     std::cout<<"Game is running"<<std::endl;
    //     game.update(0.1f);
    //     // game.draw();
    //     char key;
    //     std::cin >> key;
    //     inputHandler.handleInput(key,game);
    // }

    test_Vector3D();
    test_Block();
    test_TetrominoMovement();
    test_Grid();
    test_Game();

    std::cout << "All tests passed!" << std::endl;
    return 0;

}
