#include "Game.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

Game::Game() 
    : window(nullptr)
    , currentPlayer(Player::X)
    , gameOver(false)
    , font(nullptr)
    , currentDifficulty(Difficulty::MEDIUM) {
    
    window = new sf::RenderWindow(sf::VideoMode({400, 500}), "Jogo da Velha - SFML");
    window->setFramerateLimit(60);
    
    // Inicializar IA com dificuldade média
    aiPlayer = new AIPlayer(Player::O, currentDifficulty);
    
    font = new sf::Font();
    if (!font->openFromFile("arial.ttf")) {
        cout << "Warning: Could not load font." << endl;
        delete font;
        font = nullptr;
    }
}

Game::~Game() {
    delete window;
    delete aiPlayer;
    if (font) delete font;
}

void Game::run() {
    cout << "Game started! Dificuldade: " << 
        (currentDifficulty == Difficulty::EASY ? "FACIL" : 
         currentDifficulty == Difficulty::MEDIUM ? "MEDIO" : "DIFICIL") << endl;
    cout << "Controles:" << endl;
    cout << "- Clique no tabuleiro para jogar" << endl;
    cout << "- Tecla D: Mudar dificuldade" << endl;
    cout << "- Tecla R: Reiniciar jogo" << endl;
    
    while (window->isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    for (auto event = window->pollEvent(); event.has_value(); event = window->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window->close();
        }
        
        if (auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
            if (keyEvent->scancode == sf::Keyboard::Scan::R) {
                resetGame();
            }
            if (keyEvent->scancode == sf::Keyboard::Scan::D) {
                cycleDifficulty();
            }
        }
        
        if (auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos = mouseEvent->position;
                
                if (gameOver) {
                    resetGame();
                } else {
                    handlePlayerClick(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                }
            }
        }
    }
}

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
    
    aiPlayer->setDifficulty(currentDifficulty);
    
    cout << "Dificuldade alterada para: " << 
        (currentDifficulty == Difficulty::EASY ? "FACIL" : 
         currentDifficulty == Difficulty::MEDIUM ? "MEDIO" : "DIFICIL") << endl;
}

void Game::update() {
    // AI move
    if (currentPlayer == Player::O && !gameOver) {
        auto move = aiPlayer->getBestMove(board);
        if (move.first != -1 && board.isValidMove(move.first, move.second)) {
            cout << "AI plays at: " << move.first << ", " << move.second << endl;
            board.makeMove(move.first, move.second, currentPlayer);
            
            Player winner = board.checkWinner();
            if (winner != Player::NONE) {
                gameOver = true;
                cout << "Game Over! " << (winner == Player::X ? "X" : "O") << " wins!" << endl;
            } else if (board.isBoardFull()) {
                gameOver = true;
                cout << "Game Over! It's a draw!" << endl;
            } else {
                switchPlayer();
            }
        }
    }
}

void Game::render() {
    window->clear(sf::Color::Black);
    board.draw(*window);
    displayGameStatus();
    displayDifficulty();
    window->display();
}

void Game::handlePlayerClick(float x, float y) {
    if (currentPlayer != Player::X || gameOver) return;
    
    const float boardSize = 300.f;
    const float cellSize = boardSize / 3.f;
    const float startX = (400.f - boardSize) / 2.f;
    const float startY = 50.f;
    
    if (x >= startX && x < startX + boardSize && y >= startY && y < startY + boardSize) {
        int col = static_cast<int>((x - startX) / cellSize);
        int row = static_cast<int>((y - startY) / cellSize);
        
        if (board.isValidMove(row, col)) {
            cout << "Player X moved to: " << row << ", " << col << endl;
            board.makeMove(row, col, currentPlayer);
            
            Player winner = board.checkWinner();
            if (winner != Player::NONE) {
                gameOver = true;
                cout << "Game Over! " << (winner == Player::X ? "X" : "O") << " wins!" << endl;
            } else if (board.isBoardFull()) {
                gameOver = true;
                cout << "Game Over! It's a draw!" << endl;
            } else {
                switchPlayer();
                cout << "AI's turn..." << endl;
            }
        }
    }
}

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

void Game::displayGameStatus() {
    if (!font) return;
    
    sf::Text statusText(*font, "", 24);
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition({120.f, 370.f});
    
    if (gameOver) {
        Player winner = board.checkWinner();
        if (winner != Player::NONE) {
            statusText.setString((winner == Player::X) ? "X Venceu!" : "IA Venceu!");
        } else {
            statusText.setString("Empate!");
        }
        
        // Botão de reset
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

void Game::switchPlayer() {
    currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
}

void Game::resetGame() {
    board.reset();
    currentPlayer = Player::X;
    gameOver = false;
    cout << "Nova partida! Dificuldade: " << 
        (currentDifficulty == Difficulty::EASY ? "FACIL" : 
         currentDifficulty == Difficulty::MEDIUM ? "MEDIO" : "DIFICIL") << endl;
}