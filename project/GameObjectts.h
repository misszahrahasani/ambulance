#pragma once

#include <random>
#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

// generate random number between range
int randomRange(int begin, int end) {
  return (rand() % (end - begin)) + begin;
}

using std::vector;
using std::string;

class GameColumn {
 public:
  struct Groove {
    int y;     // top distance
    int width; //
  };

 protected:
  int _x;      // left distance
  int _height; //
  //
  vector<Groove> _grooves;

 public:
  GameColumn(int x, int height, vector<Groove> const& grooves)
    : _x(x),
      _height(height),
      _grooves(grooves) {
  }

  static GameColumn createRandom(int x, int height) {
    //
    vector<Groove> grooves;
    int y = randomRange(0, height / 3),
        width;
    do {
      y    += (int) randomRange(3, 5);
      width = (int) randomRange(1, 3);
      // check if groove is possible to create or not
      if (!grooves.empty() and y + width >= height - 2)
        break;
      grooves.push_back(Groove{y, width});
      // move to next groove
      y += width + 1;
    } while (true);
    //
    return GameColumn{x, height, grooves};
  }

  // check if point is on column(not on grooves!)
  bool onColumn(int x, int y) const {
    if (_x == x) {
      int begin = 0;
      for (auto groove : _grooves) {
        if (begin <= y and y <= groove.y - 1)
          return true;
        // move to next groove
        begin = groove.y + groove.width + 1;
      }
      if (begin < _height)
        return begin - 1 <= y;
    }
    return false;
  }

  int getX() const { return _x; }

  vector<Groove> getGrooves() const { return _grooves; }
};

class GameBoard {
 private:
  int _width,
      _height;
  vector<GameColumn> _columns;

 public:
  explicit GameBoard(int width, int height, vector<GameColumn> columns)
    : _width(width),
      _height(height),
      _columns(std::move(columns)) {
  }

  static GameBoard createRandom(int width, int height, int beginOffset, int endOffset) {
    int x = beginOffset;
    vector<GameColumn> columns;
    //
    columns.push_back(GameColumn(beginOffset, height, {GameColumn::Groove{height/2-1, 3}}));
    //
    do {
      //                     minGap   maxGap
      x += (int) randomRange(2      , 3) + 1; // gap between two columns
      //
      if (x >= width - endOffset - 3)
        break;
      columns.push_back(GameColumn::createRandom(x, height));
    } while (true);
    //
    columns.push_back(GameColumn(width-endOffset, height, {GameColumn::Groove{height/2, 1}}));
    //
    return GameBoard{width, height, columns};
  }

  bool onColumn(int x, int y) const {
    for (auto const& column : _columns)
      if (column.onColumn(x, y))
        return true;
    return false;
  }

  int getWidth()  const { return _width; }
  int getHeight() const { return _height; }

  vector<GameColumn> getColumns() const { return _columns; }
};

class GamePlayer {
 private:
  int _x,
      _y;

 public:
  GamePlayer(int x, int y)
    : _x(x),
      _y(y) {
 }

  bool move(int xO, int yO, GameBoard const& board) {
    // calculate final position
    int x = _x + xO,
        y = _y + yO;
    // check if position is on board
    if (0 <= x and x <= board.getWidth() and
        0 <= y and y < board.getHeight()) {
      // check if position isn't on column
      if (not board.onColumn(x, y)) {
        _x = x;
        _y = y;
        //
        return true;
      }
    }
    return false;
  }

  int getX() const { return _x; }
  int getY() const { return _y; }
};

class GameRender {
  const char pixelWalls  = '*',
             pixelPlayer = '@';

 private:
  GameBoard  &_board;
  GamePlayer &_player;

 public:
  GameRender(GameBoard &board, GamePlayer &player)
    : _board (board),
      _player(player) {
  }

  void render() const {
    const int width  = _board.getWidth(),
              height = _board.getHeight();
    //
    vector<string> buffer(height);
    for (string &line : buffer) {
      line = std::string(width + 1, ' ');
    }
    //
    for (auto const& column : _board.getColumns()) {
      int y = 0;
      int x = column.getX();
      int grooveIndex = 0;
      for (auto groove : column.getGrooves()) {
        if (grooveIndex)
          ++y;
        for (int i=y; i<groove.y; ++i)
          buffer[i][x] = '|';
        // draw groove borders
        if (y)
          buffer[y][x] = '+';
        buffer[groove.y-1][x] = '+';
        //
        y = groove.y + groove.width;
        // move to next groove
        ++grooveIndex;
      }
      // draw last groove border
      buffer[y][x] = '+';
      for (int i=y+1; i<height; ++i)
        buffer[i][column.getX()] = '|';
    }
    //
    buffer[_player.getY()][_player.getX()] = pixelPlayer;
    //
    string sides(width, '-');
    //
    std::cout << ' ' <<  sides << std::endl;
    for (string const& line : buffer)
      std::cout << '|' << line  << std::endl;
    std::cout << ' ' <<  sides << std::endl;
  }

  void renderWon(int seconds, int score) {
    const size_t width = _board.getWidth();
    //
    string message = "Elapsed time: " + std::to_string(seconds) + "(s), Score: " + std::to_string(score);
    size_t messageWidth = message.length();
    // print centered message
    string sides(width, '-');
    std::cout << std::endl
              << ' ' << sides << std::endl
              << '|' << std::setw((int) (width + messageWidth)/2) << message
                     << std::setw((int) (width - (width + messageWidth)/2 + 1)) << '|' << std::endl
              << ' ' << sides << std::endl;
  }

  static void clear() {
    system("cls");
  }
};