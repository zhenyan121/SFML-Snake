#include <SFML/Graphics.hpp>
#include <iostream>


constexpr int WINDOWS_WIDTH = 800;
constexpr int WINDOWS_HEIGHT = 600;
constexpr char* WINDOWS_TITLE = "SFML_Snake";
constexpr int ROW = 10;
constexpr int COL = 10;


struct World {
    std::vector<sf::RectangleShape> board;
    float cellSize;
    sf::Vector2f originPoint = {0.0f, 0.0f};
};





int main() {
    
    
    //sf::Window window(sf::VideoMode({WINDOWS_WIDTH, WINDOWS_HEIGHT}), WINDOWS_TITLE, sf::Style::Default);
    
    // 支持绘图的window派生类
    sf::RenderWindow window(sf::VideoMode({WINDOWS_WIDTH, WINDOWS_HEIGHT}), WINDOWS_TITLE, sf::Style::Default);
    // 开启垂直同步
    window.setVerticalSyncEnabled(true);
    
    //初始化游戏
    World world;

    world.board.resize(ROW * COL);
    
    world.cellSize = std::min(WINDOWS_WIDTH / COL, WINDOWS_HEIGHT / ROW);

    world.originPoint = {
        (WINDOWS_WIDTH - (world.cellSize * COL)) / 2,
        (WINDOWS_HEIGHT - (world.cellSize * ROW)) / 2
    };

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            int index = i * ROW + j;
            world.board[index].setFillColor(sf::Color::White);
            world.board[index].setOutlineThickness(1.f);
            world.board[index].setOutlineColor(sf::Color::Black);
            world.board[index].setSize({world.cellSize, world.cellSize});
            auto [X, Y] = world.originPoint;
            world.board[index].setPosition({X + j * world.cellSize, Y + i * world.cellSize});
        }
    }
    // 游戏循环


    
    while (window.isOpen()) {

        
        // 事件循环
        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

        }   

        window.clear(sf::Color::White);
        
        for (const auto& cell : world.board ) {
            window.draw(cell);
        }

        window.display();

    }


}







