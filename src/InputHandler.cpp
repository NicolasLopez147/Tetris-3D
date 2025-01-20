
#include "Game.cpp"
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