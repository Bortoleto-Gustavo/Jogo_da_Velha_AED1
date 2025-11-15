#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "Board.h"
#include <vector>
#include <utility>
#include <random>
using namespace std;

enum class Difficulty { EASY,   // IA joga quase aleatoriamente
                        MEDIUM, // IA usa estratégia simples com alguns erros
                        HARD    // IA usa Minimax para jogada quase perfeita
}; 

/*
 * @struct GameState
 * @brief Representa um nó na árvore de estados do jogo
 * 
 * Cada nó armazena um estado do tabuleiro e suas possíveis transições
 * Esta estrutura forma a base da árvore de decisão da IA
 */                        
struct GameState {
    vector<vector<Player>> board;         // Estado atual do tabuleiro 3x3
    Player currentPlayer;                           // Jogador que deve jogar neste estado
    int score;                                      // Avaliação heurística do estado
    vector<GameState*> children;               // Ponteiros para estados filhos (próximas jogadas possíveis)
    GameState* parent;                              // Ponteiro para estado pai (jogada anterior)
    
    GameState(const vector<vector<Player>>& boardState, Player player);
    ~GameState();
};

/*
 * @class AIPlayer
 * @brief Implementa a IA usando árvore de decisão e algoritmo Minimax
 * 
 * A classe constrói uma árvore onde cada nó representa um estado possível
 * do jogo e usa o algoritmo Minimax para escolher a melhor jogada.
 */
class AIPlayer {
public:
    AIPlayer(Player aiPlayer, Difficulty difficulty = Difficulty::HARD);
    ~AIPlayer();
    
    void setDifficulty(Difficulty newDifficulty);
    pair<int, int> getBestMove(Board& board);
    
private:
    Player aiPlayer;        // Peça controlada pela IA (X ou O)
    Player humanPlayer;     // Peça controlada pelo jogador humano
    GameState* root;        // Raiz da árvore de estados (estado atual do jogo)
    Difficulty difficulty;  // Nível atual de dificuldade
    mt19937 rng;       // Gerador de números aleatórios para jogadas não-determinísticas
    
    void buildGameTree(GameState* state, int depth);
    int evaluateBoard(const vector<vector<Player>>& board);
    int minimax(GameState* state, int depth, bool isMaximizing);
    vector<pair<int, int>> getAvailableMoves(const vector<vector<Player>>& board);
    Player getOpponent(Player player);
    bool isGameOver(const vector<vector<Player>>& board);
    
    pair<int, int> getRandomMove(const vector<vector<Player>>& board);
    pair<int, int> getEasyMove(const vector<vector<Player>>& board);
    pair<int, int> getMediumMove(const vector<vector<Player>>& board);
    pair<int, int> getHardMove(const vector<vector<Player>>& board);
    pair<int, int> findMoveDifference(const vector<vector<Player>>& oldBoard, const vector<vector<Player>>& newBoard);
    Player checkWinner(const vector<vector<Player>>& board);
};

#endif