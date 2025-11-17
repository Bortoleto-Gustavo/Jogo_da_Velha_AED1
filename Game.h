#ifndef GAME_H
#define GAME_H

namespace sf {
    class RenderWindow;
    class Font;
    class Text;
    class Event;
    class RectangleShape;
}

#include "Board.h" //codigo do tabuleiro do jogo.
#include "AIPlayer.h" //codigo do computador

class Game {
public:
    Game(); // construtor inicializando o jogo
    ~Game(); //destrutor;
    void run();
    
private:
    sf::RenderWindow* window;
    Board board; // objeto do tabuleiro
    AIPlayer* aiPlayer; // ponteiro para o jogador ia
    Player currentPlayer; // representa qual jogador está jogando no momento.
    bool gameOver; // indica se houve vitória/derrota
    sf::Font* font; 
    Difficulty currentDifficulty; // dificuldade atual da ia
    
    void processEvents();
    void update();
    void render(); // renderiza todos os objetos da tela.
    void handlePlayerClick(float x, float y);
    void switchPlayer(); // alterna o jogador da vez.
    void resetGame(); // reinicia o jogo.
    void displayGameStatus(); // mostra o status do jogo.
    void displayDifficulty(); // mostra a dificuldade do jogo atual.
    void cycleDifficulty(); // altera a dificuldade.
};

#endif
