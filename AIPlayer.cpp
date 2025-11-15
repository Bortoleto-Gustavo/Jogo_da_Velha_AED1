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
    : aiPlayer(aiPlayer), difficulty(difficulty) {
    
    humanPlayer = (aiPlayer == Player::X) ? Player::O : Player::X;
    root = nullptr;
    
    // Inicializar gerador de números aleatórios
    random_device rd;
    rng = mt19937(rd());
}

AIPlayer::~AIPlayer() {
    if (root) {
        delete root;
    }
}

void AIPlayer::setDifficulty(Difficulty newDifficulty) {
    difficulty = newDifficulty;
}

pair<int, int> AIPlayer::getBestMove(Board& board) {
    vector<vector<Player>> currentBoard = board.getGrid();
    
    switch(difficulty) {
        case Difficulty::EASY: 
            return getEasyMove(currentBoard);   // Estratégia quase aleatória
        case Difficulty::MEDIUM:
            return getMediumMove(currentBoard); // Estratégia baseada em regras
        case Difficulty::HARD:
            return getHardMove(currentBoard);   // Busca completa com Minimax
        default:
            return getRandomMove(currentBoard); // Fallback
    }
}

// Helper para verificar vencedor
Player AIPlayer::checkWinner(const vector<vector<Player>>& board) {
    // Check linhas
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] != Player::NONE && 
            board[i][0] == board[i][1] && 
            board[i][1] == board[i][2]) {
            return board[i][0];
        }
    }
    
    // Check colunas
    for (int i = 0; i < 3; ++i) {
        if (board[0][i] != Player::NONE && 
            board[0][i] == board[1][i] && 
            board[1][i] == board[2][i]) {
            return board[0][i];
        }
    }
    
    // Check diagonais
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

// Helper para encontrar a diferença entre dois tabuleiros
pair<int, int> AIPlayer::findMoveDifference(
    const vector<vector<Player>>& oldBoard,
    const vector<vector<Player>>& newBoard) {
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (oldBoard[i][j] != newBoard[i][j] && newBoard[i][j] == aiPlayer) {
                return {i, j};
            }
        }
    }
    return {-1, -1};
}

// Dificuldade FÁCIL: Joga quase aleatoriamente
pair<int, int> AIPlayer::getEasyMove(const vector<vector<Player>>& board) {
    auto moves = getAvailableMoves(board);
    if (moves.empty()) return {-1, -1};
    
    // 70% de chance de jogada aleatória, 30% de jogada decente
    uniform_real_distribution<float> dist(0.0f, 1.0f);
    if (dist(rng) < 0.7f) {
        // Jogada completamente aleatória
        uniform_int_distribution<int> moveDist(0, moves.size() - 1);
        return moves[moveDist(rng)];
    } else {
        // Às vezes faz uma jogada decente
        return getMediumMove(board);
    }
}

// Dificuldade MÉDIA: Joga bem mas comete erros
pair<int, int> AIPlayer::getMediumMove(const vector<vector<Player>>& board) {
    auto moves = getAvailableMoves(board);
    if (moves.empty()) return {-1, -1};
    
    // Estratégia simples para médio
    // 1. Primeiro tenta vencer
    for (const auto& move : moves) {
        auto testBoard = board;
        testBoard[move.first][move.second] = aiPlayer;
        if (checkWinner(testBoard) == aiPlayer) {
            return move;
        }
    }
    
    // 2. Tenta bloquear o jogador
    for (const auto& move : moves) {
        auto testBoard = board;
        testBoard[move.first][move.second] = humanPlayer;
        if (checkWinner(testBoard) == humanPlayer) {
            return move;
        }
    }
    
    // 3. Jogadas estratégicas com alguma aleatoriedade
    vector<pair<int, int>> goodMoves;
    
    // Centro é sempre bom
    if (board[1][1] == Player::NONE) {
        goodMoves.push_back({1, 1});
    }
    
    // Cantos são bons
    vector<pair<int, int>> corners = {{0,0}, {0,2}, {2,0}, {2,2}};
    for (const auto& corner : corners) {
        if (board[corner.first][corner.second] == Player::NONE) {
            goodMoves.push_back(corner);
        }
    }
    
    // 50% de chance de escolher uma boa jogada, 50% de jogada aleatória
    uniform_real_distribution<float> dist(0.0f, 1.0f);
    if (!goodMoves.empty() && dist(rng) < 0.5f) {
        uniform_int_distribution<int> moveDist(0, goodMoves.size() - 1);
        return goodMoves[moveDist(rng)];
    } else {
        uniform_int_distribution<int> moveDist(0, moves.size() - 1);
        return moves[moveDist(rng)];
    }
}

// Dificuldade DIFÍCIL: Usa Minimax (quase perfeito)
pair<int, int> AIPlayer::getHardMove(const vector<vector<Player>>& board) {
    root = new GameState(board, aiPlayer);
    buildGameTree(root, 6); // Profundidade alta para jogada quase perfeita
    
    int bestScore = INT_MIN;
    pair<int, int> bestMove = {-1, -1};
    vector<pair<int, int>> bestMoves;
    
    for (GameState* child : root->children) {
        int score = minimax(child, 6, false);
        if (score > bestScore) {
            bestScore = score;
            bestMoves.clear();
            bestMoves.push_back(findMoveDifference(board, child->board));
        } else if (score == bestScore) {
            bestMoves.push_back(findMoveDifference(board, child->board));
        }
    }
    
    delete root;
    root = nullptr;
    
    if (!bestMoves.empty()) {
        uniform_int_distribution<int> dist(0, bestMoves.size() - 1);
        return bestMoves[dist(rng)];
    }
    
    return getRandomMove(board);
}

pair<int, int> AIPlayer::getRandomMove(const vector<vector<Player>>& board) {
    auto moves = getAvailableMoves(board);
    if (moves.empty()) return {-1, -1};
    
    uniform_int_distribution<int> dist(0, moves.size() - 1);
    return moves[dist(rng)];
}

void AIPlayer::buildGameTree(GameState* state, int depth) {
    if (depth == 0 || isGameOver(state->board)) return;
    
    // Obter todos os movimentos possíveis a partir deste estado
    auto moves = getAvailableMoves(state->board);
    
    // Expansão: criar um filho para cada movimento possível
    for (const auto& move : moves) {
        
        // Cria uma cópia do tabuleiro atual
        vector<vector<Player>> newBoard = state->board;
        
        // Aplica o movimento no novo tabuleiro
        newBoard[move.first][move.second] = state->currentPlayer;
        
        // Cria novo nó representando o estado após o movimento
        GameState* newState = new GameState(newBoard, getOpponent(state->currentPlayer));
        newState->parent = state;
        state->children.push_back(newState);
        
        // Expansão recursiva - profundidade reduzida em 1
        buildGameTree(newState, depth - 1);
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

int AIPlayer::minimax(GameState* state, int depth, bool isMaximizing) {
    // Caso base: profundidade máxima ou estado terminal
    if (depth == 0 || state->children.empty() || isGameOver(state->board)) {
        return evaluateBoard(state->board);
    }
    
    if (isMaximizing) {
        // Perspectiva da IA - quer maximizar o score
        int bestScore = INT_MIN;
        for (GameState* child : state->children) {
            // Chamada recursiva - alterna para minimização
            int score = minimax(child, depth - 1, false);
            bestScore = max(bestScore, score);
        }
        return bestScore;
    } else {
        // Perspectiva do humano - quer minimizar o score da IA  
        int bestScore = INT_MAX;
        for (GameState* child : state->children) {
            // Chamada recursiva - alterna para maximização
            int score = minimax(child, depth - 1, true);
            bestScore = min(bestScore, score);
        }
        return bestScore;
    }
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