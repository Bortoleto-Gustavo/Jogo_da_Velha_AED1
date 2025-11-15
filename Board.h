#ifndef BOARD_H
#define BOARD_H

#include <vector>
using namespace std;
    
namespace sf {
    class RenderWindow;
}

enum class Player { NONE, X, O };


/*
 * @class Board  
 * @brief Representa o tabuleiro do jogo da velha e sua lógica
 * 
 * Responsável por:
 * - Armazenar o estado do tabuleiro 3x3
 * - Validar movimentos
 * - Verificar condições de vitória/empate
 * - Renderizar graficamente o tabuleiro
 */
class Board {
public:
    Board();
    
    void reset();
    bool makeMove(int row, int col, Player player);
    bool isValidMove(int row, int col) const;
    Player checkWinner() const;
    bool isBoardFull() const;
    Player getCell(int row, int col) const;
    const vector<vector<Player>>& getGrid() const;
    
    void draw(sf::RenderWindow& window);
    
private:
    vector<vector<Player>> grid;
};

#endif
