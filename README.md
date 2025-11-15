# Jogo da Velha com IA - SFML & Árvore Binária
## Descrição do Projeto
Este projeto foi feito para a matéria de Estruturas de Dados, buscando aplicar os conceitos de implementação de uma árvore binária.

# 📁 Estrutura do Projeto

## main.cpp
**Função:** Ponto de entrada do programa - inicia o jogo.
```
// Apenas cria e executa o jogo
Game game;
game.run();
```

## Game.h / Game.cpp
**Função:** Classe principal que controla todo o fluxo do jogo.

* Gerencia o loop principal e eventos

* Controla alternância entre jogador e IA

* Renderiza interface e botões

* Gerencia sistema de dificuldade (Tecla D)

## Board.h / Board.cpp
**Função:** Representa o tabuleiro e suas regras.
* Armazena estado do jogo (matriz 3x3)

* Verifica movimentos válidos

* Detecta vitórias/empates

* Desenha o tabuleiro gráfico (X, O e linhas)

## AIPlayer.h / AIPlayer.cpp
**Função:** Implementa a inteligência artificial com árvore de decisão.

* **Árvore de Estados:** GameState - cada nó é um possível estado do jogo

* **Algoritmo Minimax:** Busca a melhor jogada explorando a árvore

* **3 Dificuldades:**

  * **Fácil:** Jogadas quase aleatórias

  * **Médio:** Estratégia simples com alguns erros

  * **Difícil:** Minimax - quase invencível

# 🎮 Controles
| Tecla/Ação             |     Função              |
| ---------------------- |:-----------------------:|
| Mouse Left             | Fazer jogada (X)        |
| D                      | Mudar dificuldade da IA |
| R                      | Reiniciar jogo          |
| Mouse Left (after game)|Nova partida             |

# 🚀 Como Compilar
```
# Compilar o projeto
g++ -std=c++17 -o jogo_da_velha.exe main.cpp Game.cpp Board.cpp AIPlayer.cpp -lsfml-graphics-d -lsfml-window-d -lsfml-system-d -I"C:\DEV\SFML-3.0.2\include" -L"C:\DEV\SFML-3.0.2\lib"

# Executar
./jogo_da_velha.exe
```
# 📚 Recuursos Utilizadas
* **SFML 3.0:** Gráficos e interface

* **Árvore Binária:** Estrutura de decisão da IA

* **Algoritmo Minimax:** IA para jogada perfeita

* **C++17:** Linguagem principal
# 🌳 Sobre a Implementação da Árvore
A IA usa uma árvore de estados onde cada nó (GameState) representa um possível estado do tabuleiro. O algoritmo Minimax explora esta árvore recursivamente para encontrar a jogada ótima, alternando entre perspectivas de maximização (IA) e minimização (jogador).
##Estrutura da Árvore:
```
struct GameState {
    std::vector<std::vector<Player>> board;  // Estado do tabuleiro
    Player currentPlayer;                    // Jogador atual
    std::vector<GameState*> children;        // Estados filhos
    GameState* parent;                       // Estado pai
};
```
# 📊 Dificuldades da IA
| Dificuldade      | Estratégia                     | Chance de Vitória  |
| ---------------- |--------------------------------|:------------------:|
| Fácil            | 70% aleatório, 30% estratégico |🟢 Alta|            |
| Médio            | Regras simples + aleatoriedade |🔴 Média/Baixa      |
| Difícil          | Minimax completo               |💀 Impossível       |
