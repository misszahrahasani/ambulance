 #include <iostream>
#include <ctime>
#include <unistd.h>
#include <conio.h>

#include "GameObjectts.h"

const int renderWidth  = 70;
const int renderHeight = 20;

int main() {
  srand(std::time(nullptr));
  // create game elements
  auto gameBoard  = GameBoard::createRandom(renderWidth, renderHeight, 5, 0);
  auto gamePlayer = GamePlayer(2, renderHeight / 2);
  // game render
  GameRender gameRender(gameBoard, gamePlayer);
  //
  time_t beginTime = 0;
  do {
    GameRender::clear();
    gameRender.render();
    // movement with WASD
    switch(_getch()) {
      case 'w': gamePlayer.move(0 , -1, gameBoard); break; // up
      case 's': gamePlayer.move(0 , 1 , gameBoard); break; // down
      case 'd': gamePlayer.move(1 , 0 , gameBoard); break; // right
      case 'a': gamePlayer.move(-1, 0 , gameBoard); break; // left
    }
    // start the game
    if (beginTime == 0)
      beginTime = std::time(nullptr);
  } while (gamePlayer.getX() != gameBoard.getWidth());
  //
  time_t elapsedTime = std::time(nullptr) - beginTime;
  // calculate
  int score = renderWidth / 4 - elapsedTime;
  //
  GameRender::clear();
  gameRender .render();
  sleep(1);
  gameRender .renderWon(elapsedTime, score);
  sleep(5);
}
