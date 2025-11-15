#include "AIPlayer.h"
#include <algorithm>
#include <climits>
#include <iostream>
#include <chrono>
using namespace std;

GameState::GameState(const vector<vector<Player>>& boardState, Player player) 
    : board(boardState), currentPlayer(player), score(0), parent(nullptr) {
}

GameState::~GameState() {
    for (auto child : children) {
        delete child;
    }
}

AIPlayer::AIPlayer(Player aiPlayer, Difficulty difficulty) 
    : aiPlayer(aiPlayer), difficulty(difficulty), root(nullptr), current(nullptr) {
    
    humanPlayer = (aiPlayer == Player::X) ? Player::O : Player::X;
    
    random_device rd;
    rng = mt19937(rd());
}

AIPlayer::~AIPlayer() {
    if (root) {
        delete root;  // Libera toda a árvore persistente
    }
}

void AIPlayer::setDifficulty(Difficulty newDifficulty) {
    difficulty = newDifficulty;
    cout << "IA: Dificuldade alterada para " 
              << (difficulty == Difficulty::EASY ? "FACIL" : 
                  difficulty == Difficulty::MEDIUM ? "MEDIO" : "DIFICIL") 
              << endl;
}


/*
 * @brief Reinicia a árvore para um novo jogo
 */
void AIPlayer::resetTree() {
    if (root) {
        delete root;  // Libera árvore antiga
    }
    
    // Cria novo estado inicial
    vector<vector<Player>> emptyBoard(3, 
        vector<Player>(3, Player::NONE));
    
    root = new GameState(emptyBoard, Player::X);
    current = root;
    
    // Constrói árvore completa apenas uma vez
    buildGameTree(root, 6);
    cout << "Árvore persistente construída" << endl;
}

/*
 * @brief Atualiza a árvore com a jogada realizada - mantém estrutura
 */
void AIPlayer::updateTree(const pair<int, int>& move) {
    // Se não temos árvore ainda, constrói
    if (root == nullptr) {
        resetTree();
        return;
    }
    
    // Procura o filho que corresponde à jogada realizada e muda para ele
    for (GameState* child : current->children) {
        pair<int, int> childMove = findMoveDifference(current->board, child->board);
        if (childMove.first == move.first && childMove.second == move.second) {
            current = child;
            cout << "Árvore atualizada: navegou para nó filho (" 
                      << move.first << "," << move.second << ")" << endl;
            return;
        }
    }
    
    // Jogada não encontrada
    cout << "AVISO: Jogada (" << move.first << "," << move.second 
              << ") não encontrada na árvore. Reiniciando árvore..." << endl;
    resetTree();
}

/*
 * @brief Calcula melhor jogada usando árvore
 */
pair<int, int> AIPlayer::getBestMove() {
    // Se não temos árvore, constrói
    if (root == nullptr) {
        resetTree();
    }
    
    // Verifica se é a vez da IA no estado atual
    if (current->currentPlayer != aiPlayer) {
        return getRandomMove();
    }
    
    // Delega para estratégia baseada na dificuldade
    switch(difficulty) {
        case Difficulty::EASY:
            return getEasyMove();
        case Difficulty::MEDIUM:
            return getMediumMove();
        case Difficulty::HARD:
            return getHardMove();
        default:
            return getRandomMove();
    }
}

// ==================== ESTRATÉGIAS USANDO ÁRVORE PERSISTENTE ====================

pair<int, int> AIPlayer::getHardMove() {
    if (current == nullptr || current->children.empty()) {
        return getRandomMove();
    }
    
    int bestScore = INT_MIN;
    pair<int, int> bestMove = {-1, -1};
    vector<pair<int, int>> bestMoves;
    
    // Avalia cada jogada possível a partir do estado atual
    for (GameState* child : current->children) {
        int score = minimax(child, 6, false);  // Profundidade reduzida para performance
        
        if (score > bestScore) {
            bestScore = score;
            bestMoves.clear();
            bestMoves.push_back(findMoveDifference(current->board, child->board));
        } else if (score == bestScore) {
            bestMoves.push_back(findMoveDifference(current->board, child->board));
        }
    }
    
    if (!bestMoves.empty()) {
        uniform_int_distribution<int> dist(0, bestMoves.size() - 1);
        return bestMoves[dist(rng)];
    }
    
    return getRandomMove();
}

pair<int, int> AIPlayer::getMediumMove() {
    if (current == nullptr) return getRandomMove();
    
    auto moves = getAvailableMoves(current->board);
    if (moves.empty()) return {-1, -1};
    
    // 1. Tenta vitória imediata
    for (const auto& move : moves) {
        auto testBoard = current->board;
        testBoard[move.first][move.second] = aiPlayer;
        if (checkWinner(testBoard) == aiPlayer) {
            return move;
        }
    }
    
    // 2. Tenta bloquear jogador
    for (const auto& move : moves) {
        auto testBoard = current->board;
        testBoard[move.first][move.second] = humanPlayer;
        if (checkWinner(testBoard) == humanPlayer) {
            return move;
        }
    }
    
    // 3. Estratégia posicional com aleatoriedade
    vector<pair<int, int>> goodMoves;
    
    if (current->board[1][1] == Player::NONE) {
        goodMoves.push_back({1, 1});
    }
    
    vector<pair<int, int>> corners = {{0,0}, {0,2}, {2,0}, {2,2}};
    for (const auto& corner : corners) {
        if (current->board[corner.first][corner.second] == Player::NONE) {
            goodMoves.push_back(corner);
        }
    }
    
    uniform_real_distribution<float> dist(0.0f, 1.0f);
    if (!goodMoves.empty() && dist(rng) < 0.5f) {
        uniform_int_distribution<int> moveDist(0, goodMoves.size() - 1);
        return goodMoves[moveDist(rng)];
    } else {
        uniform_int_distribution<int> moveDist(0, moves.size() - 1);
        return moves[moveDist(rng)];
    }
}

pair<int, int> AIPlayer::getEasyMove() {
    if (current == nullptr) return getRandomMove();
    
    auto moves = getAvailableMoves(current->board);
    if (moves.empty()) return {-1, -1};
    
    uniform_real_distribution<float> dist(0.0f, 1.0f);
    if (dist(rng) < 0.7f) {
        uniform_int_distribution<int> moveDist(0, moves.size() - 1);
        return moves[moveDist(rng)];
    } else {
        return getMediumMove();
    }
}

pair<int, int> AIPlayer::getRandomMove() {
    auto moves = getAvailableMoves(current->board);
    if (moves.empty()) return {-1, -1};
    
    uniform_int_distribution<int> dist(0, moves.size() - 1);
    return moves[dist(rng)];
}

// ==================== MÉTODOS DA ÁRVORE ====================

void AIPlayer::buildGameTree(GameState* state, int depth) {
    if (depth == 0 || isGameOver(state->board)) return;
    
    auto moves = getAvailableMoves(state->board);
    for (const auto& move : moves) {
        vector<vector<Player>> newBoard = state->board;
        newBoard[move.first][move.second] = state->currentPlayer;
        
        GameState* newState = new GameState(newBoard, getOpponent(state->currentPlayer));
        newState->parent = state;
        state->children.push_back(newState);
        
        buildGameTree(newState, depth - 1);
    }
}

int AIPlayer::minimax(GameState* state, int depth, bool isMaximizing) {
    if (depth == 0 || state->children.empty() || isGameOver(state->board)) {
        return evaluateBoard(state->board);
    }
    
    if (isMaximizing) {
        int bestScore = INT_MIN;
        for (GameState* child : state->children) {
            int score = minimax(child, depth - 1, false);
            bestScore = max(bestScore, score);
        }
        return bestScore;
    } else {
        int bestScore = INT_MAX;
        for (GameState* child : state->children) {
            int score = minimax(child, depth - 1, true);
            bestScore = min(bestScore, score);
        }
        return bestScore;
    }
}

bool AIPlayer::isGameOver(const vector<vector<Player>>& board) {
    if (checkWinner(board) != Player::NONE) return true;
    
    for (const auto& row : board) {
        for (const auto& cell : row) {
            if (cell == Player::NONE) return false;
        }
    }
    return true;
}

int AIPlayer::evaluateBoard(const vector<vector<Player>>& board) {
    Player winner = checkWinner(board);
    if (winner == aiPlayer) return 10;
    if (winner == humanPlayer) return -10;
    return 0;
}

vector<pair<int, int>> AIPlayer::getAvailableMoves(const vector<vector<Player>>& board) {
    vector<pair<int, int>> moves;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == Player::NONE) {
                moves.push_back({i, j});
            }
        }
    }
    return moves;
}

Player AIPlayer::getOpponent(Player player) {
    return (player == Player::X) ? Player::O : Player::X;
}

pair<int, int> AIPlayer::findMoveDifference(
    const vector<vector<Player>>& oldBoard,
    const vector<vector<Player>>& newBoard) {
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (oldBoard[i][j] != newBoard[i][j]) {
                return {i, j};
            }
        }
    }
    return {-1, -1};
}

Player AIPlayer::checkWinner(const vector<vector<Player>>& board) {
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] != Player::NONE && 
            board[i][0] == board[i][1] && 
            board[i][1] == board[i][2]) {
            return board[i][0];
        }
    }
    
    for (int i = 0; i < 3; ++i) {
        if (board[0][i] != Player::NONE && 
            board[0][i] == board[1][i] && 
            board[1][i] == board[2][i]) {
            return board[0][i];
        }
    }
    
    if (board[0][0] != Player::NONE && 
        board[0][0] == board[1][1] && 
        board[1][1] == board[2][2]) {
        return board[0][0];
    }
    if (board[0][2] != Player::NONE && 
        board[0][2] == board[1][1] && 
        board[1][1] == board[2][0]) {
        return board[0][2];
    }
    
    return Player::NONE;
}
