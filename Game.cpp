#include "Game.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

// Construtor do jogo: inicializa janela, IA, jogador inicial e fonte
Game::Game() 
    : window(nullptr)
    , aiPlayer(nullptr)
    , currentPlayer(Player::X)          // Jogador humano começa
    , gameOver(false)
    , font(nullptr)
    , currentDifficulty(Difficulty::MEDIUM) {  // Dificuldade padrão
    
    // Cria janela SFML
    window = new sf::RenderWindow(sf::VideoMode({400, 500}), "Jogo da Velha - SFML (Árvore Persistente)");
    window->setFramerateLimit(60);
    
    // Cria IA controlando o jogador O
    aiPlayer = new AIPlayer(Player::O, currentDifficulty);
    
    // Carrega fonte
    font = new sf::Font();
    if (!font->openFromFile("arial.ttf")) {
        cout << "Warning: Could not load font." << endl;
        delete font;
        font = nullptr;
    }
}

// Destrutor: libera memória
Game::~Game() {
    delete window;
    delete aiPlayer;
    if (font) delete font;
}

// Loop principal do jogo
void Game::run() {
    cout << "=== JOGO DA VELHA ===" << endl;
    cout << "Dificuldade: " << 
        (currentDifficulty == Difficulty::EASY ? "FACIL" : 
         currentDifficulty == Difficulty::MEDIUM ? "MEDIO" : "DIFICIL") << endl;
    cout << "Controles:" << endl;
    cout << "- Clique no tabuleiro para jogar" << endl;
    cout << "- Tecla D: Mudar dificuldade" << endl;
    cout << "- Tecla R: Reiniciar jogo" << endl;
    
    // Loop enquanto a janela está aberta
    while (window->isOpen()) {
        processEvents();   // Lida com eventos do teclado e mouse
        update();          // Atualiza lógica (movimento da IA)
        render();          // Desenha tudo na tela
    }
}

// Processa eventos do usuário
void Game::processEvents() {
    for (auto event = window->pollEvent(); event.has_value(); event = window->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window->close();   // Fecha janela
        }
        
        // Eventos de teclado
        if (auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
            if (keyEvent->scancode == sf::Keyboard::Scan::R) {
                resetGame();   // Reinicia partida
            }
            if (keyEvent->scancode == sf::Keyboard::Scan::D) {
                cycleDifficulty();  // Alterna dificuldade
            }
        }
        
        // Clique do mouse
        if (auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos = mouseEvent->position;
                
                if (gameOver) {    // Se já acabou, clique inicia novo jogo
                    resetGame();
                } else {
                    handlePlayerClick(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                }
            }
        }
    }
}

// Alterna entre fácil → médio → difícil → fácil
void Game::cycleDifficulty() {
    switch(currentDifficulty) {
        case Difficulty::EASY:
            currentDifficulty = Difficulty::MEDIUM;
            break;
        case Difficulty::MEDIUM:
            currentDifficulty = Difficulty::HARD;
            break;
        case Difficulty::HARD:
            currentDifficulty = Difficulty::EASY;
            break;
    }
    
    aiPlayer->setDifficulty(currentDifficulty);  // Atualiza IA
    
    cout << "Dificuldade alterada para: " << 
        (currentDifficulty == Difficulty::EASY ? "FACIL" : 
         currentDifficulty == Difficulty::MEDIUM ? "MEDIO" : "DIFICIL") << endl;
}

// Atualiza lógica do jogo: turno da IA
void Game::update() {
    if (currentPlayer == Player::O && !gameOver) {
        auto move = aiPlayer->getBestMove();  // IA escolhe movimento
        
        if (move.first != -1 && board.isValidMove(move.first, move.second)) {
            cout << "AI plays at: " << move.first << ", " << move.second << endl;
            board.makeMove(move.first, move.second, currentPlayer);
            
            // IA atualiza árvore persistente
            aiPlayer->updateTree(move);
            
            // Verifica ganhador ou empate
            Player winner = board.checkWinner();
            if (winner != Player::NONE) {
                gameOver = true;
                cout << "Game Over! " << (winner == Player::X ? "X" : "O") << " wins!" << endl;
            } else if (board.isBoardFull()) {
                gameOver = true;
                cout << "Game Over! It's a draw!" << endl;
            } else {
                switchPlayer();  // Volta para o jogador humano
            }
        }
    }
}

// Desenha tela completa
void Game::render() {
    window->clear(sf::Color::Black);
    board.draw(*window);       // Desenha tabuleiro
    displayGameStatus();       // Texto de status
    displayDifficulty();       // Texto da dificuldade
    window->display();
}

// Trata clique do jogador humano
void Game::handlePlayerClick(float x, float y) {
    if (currentPlayer != Player::X || gameOver) return;
    
    // Dimensões do tabuleiro
    const float boardSize = 300.f;
    const float cellSize = boardSize / 3.f;
    const float startX = (400.f - boardSize) / 2.f;
    const float startY = 50.f;
    
    // Verifica se clique ocorreu dentro do tabuleiro
    if (x >= startX && x < startX + boardSize && y >= startY && y < startY + boardSize) {
        int col = static_cast<int>((x - startX) / cellSize);
        int row = static_cast<int>((y - startY) / cellSize);
        
        if (board.isValidMove(row, col)) {
            cout << "Player X moved to: " << row << ", " << col << endl;
            board.makeMove(row, col, currentPlayer);
            
            // Atualiza árvore persistente
            aiPlayer->updateTree({row, col});
            
            // Checa fim de jogo
            Player winner = board.checkWinner();
            if (winner != Player::NONE) {
                gameOver = true;
            } else if (board.isBoardFull()) {
                gameOver = true;
            } else {
                switchPlayer();  // Passa turno para IA
                cout << "AI's turn..." << endl;
            }
        }
    }
}

// Mostra dificuldade atual
void Game::displayDifficulty() {
    if (!font) return;
    
    sf::Text diffText(*font, "", 16);
    diffText.setFillColor(sf::Color::Yellow);
    diffText.setPosition({10.f, 10.f});
    
    string diffStr;
    switch(currentDifficulty) {
        case Difficulty::EASY: diffStr = "Dificuldade: FACIL (Tecla D)"; break;
        case Difficulty::MEDIUM: diffStr = "Dificuldade: MEDIO (Tecla D)"; break;
        case Difficulty::HARD: diffStr = "Dificuldade: DIFICIL (Tecla D)"; break;
    }
    
    diffText.setString(diffStr);
    window->draw(diffText);
}

// Mostra status: vez do jogador, vitória, empate, etc
void Game::displayGameStatus() {
    if (!font) return;
    
    sf::Text statusText(*font, "", 24);
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition({120.f, 370.f});
    
    if (gameOver) {
        Player winner = board.checkWinner();
        
        // Define texto principal
        if (winner != Player::NONE) {
            statusText.setString((winner == Player::X) ? "X Venceu!" : "IA Venceu!");
        } else {
            statusText.setString("Empate!");
        }
        
        // Desenha botão de nova partida
        sf::RectangleShape button({200.f, 40.f});
        button.setFillColor(sf::Color::Green);
        button.setPosition({100.f, 400.f});
        window->draw(button);
        
        sf::Text buttonText(*font, "Nova Partida", 20);
        buttonText.setFillColor(sf::Color::Black);
        buttonText.setPosition({130.f, 405.f});
        window->draw(buttonText);
        
    } else {
        statusText.setString((currentPlayer == Player::X) ? "Sua vez (X)" : "Vez da IA (O)");
    }
    
    window->draw(statusText);
}

// Alterna jogador X <-> O
void Game::switchPlayer() {
    currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
}

// Reinicia toalmente o jogo
void Game::resetGame() {
    board.reset();
    currentPlayer = Player::X;
    gameOver = false;
    
    // Reset da árvore persistente
    aiPlayer->resetTree();
    
    cout << "=== NOVA PARTIDA ===" << endl;
    cout << "Dificuldade: " << 
        (currentDifficulty == Difficulty::


