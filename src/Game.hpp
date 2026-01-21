#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_set>

inline constexpr int WINDOWS_WIDTH = 800;
inline constexpr int WINDOWS_HEIGHT = 600;
inline constexpr int ROW = 10;
inline constexpr int COL = 10;
inline constexpr const char* WINDOWS_TITLE = "SFML_Snake";


struct World {
    std::vector<sf::RectangleShape> board;
    float cellSize;
    sf::Vector2f originPoint = {0.0f, 0.0f};

    sf::Vector2i pellet;

    std::unordered_set<int> blank;

};

struct Snake {
    // 这里的第一个参数是row 第二个是col，不要和坐标的x与y搞混了
    std::vector<sf::Vector2i> node;
    sf::Vector2i direction = {0, 1};

    const float moveTime = 0.18f;

    float currentTime = 0.f;

};

enum class GameState {
    GAME_RUNING,
    GAME_LOSE,
    GAME_WIN
};

inline int getIndex(int row, int col) {
    return row * ROW + col; 
}