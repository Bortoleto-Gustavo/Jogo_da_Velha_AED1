#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "Board.h"
#include <vector>
#include <utility>
#include <random>

using namespace std;

enum class Difficulty { 
    EASY,   // IA joga quase aleatoriamente
    MEDIUM, // IA usa estratégia simples com alguns erros
    HARD    // IA usa Minimax para jogada quase perfeita
};

/*
 * @struct GameState
 * @brief Representa um nó na árvore de estados do jogo
 */
struct GameState {
    vector<vector<Player>> board;               // Estado atual do tabuleiro 3x3
    Player currentPlayer;                       // Jogador que deve jogar neste estado
    int score;                                  // Avaliação heurística do estado
    vector<GameState*> children;                // Ponteiros para estados filhos
    GameState* parent;                          // Ponteiro para estado pai
    
    GameState(const vector<vector<Player>>& boardState, Player player);
    ~GameState();
};

/*
 * @class AIPlayer
 * @brief Implementa a IA usando árvore de decisão persistente
 */
class AIPlayer {
public:
    AIPlayer(Player aiPlayer, Difficulty difficulty = Difficulty::HARD);
    ~AIPlayer();
    
    void setDifficulty(Difficulty newDifficulty);
    pair<int, int> getBestMove();
    void updateTree(const pair<int, int>& move);
    void resetTree();
    
private:
    Player aiPlayer;
    Player humanPlayer;
    GameState* root;        // Raiz persistente da árvore
    GameState* current;     // Estado atual na árvore
    Difficulty difficulty;
    mt19937 rng;
    
    // Métodos da árvore
    void buildGameTree(GameState* state, int depth);
    int evaluateBoard(const vector<vector<Player>>& board);
    int minimax(GameState* state, int depth, bool isMaximizing);
    vector<pair<int, int>> getAvailableMoves(const vector<vector<Player>>& board);
    Player getOpponent(Player player);
    bool isGameOver(const vector<vector<Player>>& board);
    
    // Estratégias por dificuldade
    pair<int, int> getRandomMove();
    pair<int, int> getEasyMove();
    pair<int, int> getMediumMove();
    pair<int, int> getHardMove();
    
    // Métodos auxiliares
    pair<int, int> findMoveDifference(const vector<vector<Player>>& oldBoard, const vector<vector<Player>>& newBoard);
    Player checkWinner(const vector<vector<Player>>& board);
};

#endif
