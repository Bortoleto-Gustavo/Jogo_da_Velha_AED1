#include "Board.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
using namespace std;

Board::Board() : grid(3, vector<Player>(3, Player::NONE)) {}

void Board::reset() {
    for (auto& row : grid) {
        fill(row.begin(), row.end(), Player::NONE);
    }
}

bool Board::makeMove(int row, int col, Player player) {
    if (isValidMove(row, col)) {
        grid[row][col] = player;
        return true;
    }
    return false;
}

bool Board::isValidMove(int row, int col) const {
    return row >= 0 && row < 3 && col >= 0 && col < 3 && grid[row][col] == Player::NONE;
}

Player Board::checkWinner() const {
    // Check linhas
    for (int i = 0; i < 3; ++i) {
        if (grid[i][0] != Player::NONE && 
            grid[i][0] == grid[i][1] && 
            grid[i][1] == grid[i][2]) {
            return grid[i][0];
        }
    }
    
    // Check colunas
    for (int i = 0; i < 3; ++i) {
        if (grid[0][i] != Player::NONE && 
            grid[0][i] == grid[1][i] && 
            grid[1][i] == grid[2][i]) {
            return grid[0][i];
        }
    }
    
    // Check diagonais
    if (grid[0][0] != Player::NONE && 
        grid[0][0] == grid[1][1] && 
        grid[1][1] == grid[2][2]) {
        return grid[0][0];
    }
    if (grid[0][2] != Player::NONE && 
        grid[0][2] == grid[1][1] && 
        grid[1][1] == grid[2][0]) {
        return grid[0][2];
    }
    
    return Player::NONE;
}

bool Board::isBoardFull() const {
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            if (cell == Player::NONE) {
                return false;
            }
        }
    }
    return true;
}

Player Board::getCell(int row, int col) const {
    if (row >= 0 && row < 3 && col >= 0 && col < 3) {
        return grid[row][col];
    }
    return Player::NONE;
}

const vector<vector<Player>>& Board::getGrid() const {
    return grid;
}

void Board::draw(sf::RenderWindow& window) {
    // Tabuleiro
    const float boardSize = 300.f;
    const float cellSize = boardSize / 3.f;
    const float startX = (400.f - boardSize) / 2.f;
    const float startY = 50.f;
    
    // Desenhar linhas do tabuleiro
    sf::RectangleShape line;
    line.setFillColor(sf::Color::White);
    
    line.setSize({5.f, boardSize});
    line.setPosition({startX + cellSize, startY});
    window.draw(line);
    line.setPosition({startX + 2 * cellSize, startY});
    window.draw(line);
    
    line.setSize({boardSize, 5.f});
    line.setPosition({startX, startY + cellSize});
    window.draw(line);
    line.setPosition({startX, startY + 2 * cellSize});
    window.draw(line);
    
    // Desenhar X e O
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            float centerX = startX + col * cellSize + cellSize / 2.f;
            float centerY = startY + row * cellSize + cellSize / 2.f;
            float size = cellSize * 0.3f;
            
            if (grid[row][col] == Player::X) {
                // X usando duas linhas que se cruzam no centro
                // Linha 1: de NO para SE
                sf::RectangleShape line1({5.f, size * 2.8f});
                line1.setFillColor(sf::Color::Red);
                line1.setPosition({centerX + size * 1.0f, centerY - size*1.1f});
                line1.setRotation(sf::degrees(45.f));
                window.draw(line1);
                
                // Linha 2: de NE para SO
                sf::RectangleShape line2({5.f, size * 2.8f});
                line2.setFillColor(sf::Color::Red);
                line2.setPosition({centerX - size * 0.9f, centerY - size});
                line2.setRotation(sf::degrees(-45.f));
                window.draw(line2);
                
            } else if (grid[row][col] == Player::O) {
                // O
                sf::CircleShape circle(size);
                circle.setFillColor(sf::Color::Transparent);
                circle.setOutlineColor(sf::Color::Blue);
                circle.setOutlineThickness(5.f);
                circle.setPosition({centerX - size, centerY - size});
                window.draw(circle);
            }
        }
    }
}
