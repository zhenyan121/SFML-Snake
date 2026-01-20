#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

constexpr int WINDOWS_WIDTH = 800;
constexpr int WINDOWS_HEIGHT = 600;
constexpr char* WINDOWS_TITLE = "SFML_Snake";
constexpr int ROW = 10;
constexpr int COL = 10;

struct World {
    std::vector<sf::RectangleShape> board;
    float cellSize;
    sf::Vector2f originPoint = {0.0f, 0.0f};

    sf::Vector2i pellet;
};

struct Snake {
    // 这里的第一个参数是row 第二个是col，不要和坐标的x与y搞混了
    std::vector<sf::Vector2i> node;
    sf::Vector2i direction = {0, 1};

    const float moveTime = 0.2f;

    float currentTime = 0.f;

};

inline int getIndex(int row, int col) {
    return row * ROW + col; 
}

inline void setBoardColor(sf::Vector2i pos, std::vector<sf::RectangleShape>& board, const sf::Color& color) {
    int index = getIndex(pos.x, pos.y);
    board[index].setFillColor(color);
}

void update (float deltaTime, World& world, Snake& snake);

bool spawnPellet(World& world, Snake& snake);


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
            int index = getIndex(i, j);
            world.board[index].setFillColor(sf::Color::White);
            world.board[index].setOutlineThickness(1.f);
            world.board[index].setOutlineColor(sf::Color::Black);
            world.board[index].setSize({world.cellSize, world.cellSize});
            auto [X, Y] = world.originPoint;
            world.board[index].setPosition({X + j * world.cellSize, Y + i * world.cellSize});
        }
    }
    // 创建蛇
    Snake snake;

    snake.node.push_back({ROW / 2, COL / 2});
    snake.node.push_back({snake.node[0].x, snake.node[0].y - 1});
    snake.node.push_back({snake.node[0].x, snake.node[0].y - 2});
    setBoardColor(snake.node[0], world.board, sf::Color::Red);
    setBoardColor(snake.node[1], world.board, sf::Color::Yellow);
    setBoardColor(snake.node[2], world.board, sf::Color::Yellow);   

    sf::Clock clock;
    // 游戏循环

    std::srand(std::time(0));

    spawnPellet(world, snake);

    while (window.isOpen()) {

        
        // 事件循环
        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                switch (keyReleased->scancode) {
                    case (sf::Keyboard::Scan::Up):
                        if (snake.node[0]  + (sf::Vector2i){-1, 0} != snake.node[1])
                            snake.direction = {-1, 0};
                        break;
                    case (sf::Keyboard::Scan::Down):
                        if (snake.node[0]  + (sf::Vector2i){1, 0} != snake.node[1])
                            snake.direction = {1, 0};
                        break;
                    case (sf::Keyboard::Scan::Right):
                        if (snake.node[0]  + (sf::Vector2i){0, 1} != snake.node[1])
                            snake.direction = {0, 1};
                        break;
                    case (sf::Keyboard::Scan::Left):
                        if (snake.node[0]  + (sf::Vector2i){0, -1} != snake.node[1])
                        snake.direction = {0, -1};
                        break;
                    default:
                        std::cout << "Unknow key input\n";
                }
            }

        }   

        update(clock.restart().asSeconds(), world, snake);

        window.clear(sf::Color::White);
        // 渲染世界网格
        for (const auto& cell : world.board ) {
            window.draw(cell);
        }

        window.display();

    }


}



void update (float deltaTime, World& world, Snake& snake) {

    

    snake.currentTime += deltaTime;
    if (snake.currentTime >= snake.moveTime) {
        snake.currentTime = 0;
        
        auto oldTail = snake.node[snake.node.size() - 1];
        
        // 更新蛇的位置和颜色
        //std::cout << snake.node.size() << "\n";
        // 更新旧尾部的颜色
        setBoardColor(snake.node[snake.node.size() - 1], world.board, sf::Color::White);
        for (int i = snake.node.size() - 1; i > 0 ; i--) {
            
            snake.node[i] = snake.node[i - 1];
        }
        // 更新旧头部的颜色 
        setBoardColor(snake.node[0], world.board, sf::Color::Yellow);
        snake.node[0] += snake.direction;
        
        //处理边界情况
        if (snake.node[0].x >= ROW) {
            snake.node[0].x = 0;
        }
        if (snake.node[0].x < 0) {
            snake.node[0].x = ROW - 1;
        }
        if (snake.node[0].y >= COL) {
            snake.node[0].y = 0;
        }
        if (snake.node[0].y < 0) {
            snake.node[0].y = COL - 1;
        }
        
        // 如果吃到豆子
        if (snake.node[0] == world.pellet) {
            spawnPellet(world, snake);
            setBoardColor(oldTail, world.board, sf::Color::Yellow);
            snake.node.push_back(oldTail);
        }
        auto [X, Y] = snake.node[0];
        // 更新新头部的颜色
        world.board[getIndex(X, Y)].setFillColor(sf::Color::Red);
    }

}

bool spawnPellet(World& world, Snake& snake) {
    bool isSpwan = false;
    
    while (!isSpwan) {
        int pelletPos = rand() % (ROW * COL);
        int row = pelletPos / COL;
        int col = pelletPos % COL;
        int index = getIndex(row, col);

        if (world.board[index].getFillColor() == sf::Color::White) {
            isSpwan = true;
            world.pellet = {row, col};
            world.board[index].setFillColor(sf::Color::Green);
        }
    }
    return true;
}





