#ifndef GAME_H
#define GAME_H

namespace sf {
    class RenderWindow;
    class Font;
    class Text;
    class Event;
    class RectangleShape;
}

#include "Board.h"
#include "AIPlayer.h"

class Game {
public:
    Game();
    ~Game();
    void run();
    
private:
    sf::RenderWindow* window;
    Board board;
    AIPlayer* aiPlayer;
    Player currentPlayer;
    bool gameOver;
    sf::Font* font;
    Difficulty currentDifficulty;
    
    void processEvents();
    void update();
    void render();
    void handlePlayerClick(float x, float y);
    void switchPlayer();
    void resetGame();
    void displayGameStatus();
    void displayDifficulty();
    void cycleDifficulty();
};

#endif
