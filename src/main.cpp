#include "Game.hpp"
#include "AI.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>




inline void setBoardColor(sf::Vector2i pos, std::vector<sf::RectangleShape>& board, const sf::Color& color) {
    int index = getIndex(pos.x, pos.y);
    board[index].setFillColor(color);
}

void update (float deltaTime, World& world, Snake& snake, GameState& gameState, sf::Text& gameOverText);

bool spawnPellet(World& world, Snake& snake);

bool loadSystemFont(sf::Font& font);

int main() {
    
    
    //sf::Window window(sf::VideoMode({WINDOWS_WIDTH, WINDOWS_HEIGHT}), WINDOWS_TITLE, sf::Style::Default);
    
    // 支持绘图的window派生类
    sf::RenderWindow window(sf::VideoMode({WINDOWS_WIDTH, WINDOWS_HEIGHT}), WINDOWS_TITLE, sf::Style::Default);
    // 开启垂直同步
    window.setVerticalSyncEnabled(true);
    
    //初始化游戏

    GameState gameState = GameState::GAME_RUNING;

    World world;

    world.board.resize(ROW * COL);
    world.blank.reserve(ROW * COL);    
    world.cellSize = std::min(WINDOWS_WIDTH / COL, WINDOWS_HEIGHT / ROW);

    world.originPoint = {
        (WINDOWS_WIDTH - (world.cellSize * COL)) / 2,
        (WINDOWS_HEIGHT - (world.cellSize * ROW)) / 2
    };


    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            int index = getIndex(i, j);
            world.blank.insert(index);
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
    for (int i = 0; i < 3; i++) {
        world.blank.erase(getIndex(snake.node[i].x, snake.node[i].y));
    }

    sf::Clock clock;
    

    std::srand(std::time(0));

    spawnPellet(world, snake);

    sf::Font font;
    if (!loadSystemFont(font)) {
        std::cerr << "load font failed\n";
    }
    sf::Text gameOverText(font);
    gameOverText.setCharacterSize(24);

    gameOverText.setPosition({WINDOWS_WIDTH / 2, WINDOWS_HEIGHT / 2});

    gameOverText.setFillColor(sf::Color::Black);

    gameOverText.setStyle(sf::Text::Bold);

    // 游戏循环
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
        if (gameState == GameState::GAME_RUNING) {
            update(clock.restart().asSeconds(), world, snake, gameState, gameOverText);
        }

        window.clear(sf::Color::White);
        // 渲染世界网格
        for (const auto& cell : world.board ) {
            window.draw(cell);
        }

        // 渲染结束画面
        if (gameState != GameState::GAME_RUNING) {
            window.draw(gameOverText);
        }
        window.display();

    }


}



void update (float deltaTime, World& world, Snake& snake, GameState& gameState, sf::Text& gameOverText) {

    

    snake.currentTime += deltaTime;
    if (snake.currentTime >= snake.moveTime) {
        snake.currentTime = 0;
        static std::queue<sf::Vector2i> action;
        if (action.empty()) {
            decideNextMove(action, world, snake);
        }
        if (!action.empty()) {
            if (action.front() != sf::Vector2i{0, 0}) {
                snake.direction = action.front();
            }
            std::cout << "direction (" << action.front().x << " " << action.front().y << ")\n";
            action.pop();
        } 
        auto oldTail = snake.node[snake.node.size() - 1];
        
        // 更新蛇的位置和颜色
        //std::cout << snake.node.size() << "\n";
        // 更新旧尾部的颜色
        setBoardColor(snake.node[snake.node.size() - 1], world.board, sf::Color::White);
        world.blank.insert(getIndex(oldTail.x, oldTail.y));
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
        // 如果碰到身体

        for (int i = 1; i < snake.node.size(); i++) {
            if (snake.node[0] == snake.node[i]) {
                gameState = GameState::GAME_LOSE;
                std::cout << "GameOver, you lose\n";
                gameOverText.setString("You, lose!");
            }
        }

        world.blank.erase(getIndex(snake.node[0].x, snake.node[0].y));
        // 如果吃到豆子
        if (snake.node[0] == world.pellet) {
            setBoardColor(oldTail, world.board, sf::Color::Yellow);
            snake.node.push_back(oldTail);
            world.blank.erase(getIndex(oldTail.x, oldTail.y));
            if(!spawnPellet(world, snake)) {
                gameState = GameState::GAME_WIN;
                gameOverText.setString("You, win");
            }
        }

        

        auto [X, Y] = snake.node[0];
        // 更新新头部的颜色

        world.board[getIndex(X, Y)].setFillColor(sf::Color::Red);
    }

}

bool spawnPellet(World& world, Snake& snake) {
    
    
    if (!world.blank.empty()) {
        int pelletIndex = rand() % (world.blank.size());
        auto it = std::next(world.blank.begin(), pelletIndex);
        int row = *it / COL;
        int col = *it % COL;
        world.pellet = {row, col};
        world.board[*it].setFillColor(sf::Color::Green);
        return true;
    }
    //auto
    
    return false;
}

bool loadSystemFont(sf::Font& font) {
    std::vector<std::string> fontPaths;
    
    #ifdef _WIN32
    // Windows系统字体路径
    fontPaths = {
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\arialbd.ttf",    // Arial Bold
        "C:\\Windows\\Fonts\\tahoma.ttf",
        "C:\\Windows\\Fonts\\verdana.ttf",
        "C:\\Windows\\Fonts\\cour.ttf",       // Courier New
        "C:\\Windows\\Fonts\\times.ttf",      // Times New Roman
    };
    
    #elif __APPLE__
    // macOS系统字体路径
    fontPaths = {
        "/System/Library/Fonts/SFNS.ttf",       // San Francisco (系统默认)
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Times.ttc",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Courier.ttc",
    };
    
    #elif __linux__
    // Linux系统字体路径（不同发行版路径不同）
    fontPaths = {
        // Debian/Ubuntu
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        // Fedora/RHEL
        "/usr/share/fonts/liberation-sans/LiberationSans-Regular.ttf",
        // Arch
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        // 通用
        "/usr/share/fonts/TTF/arial.ttf",
        "/usr/share/fonts/TTF/tahoma.ttf",
        "/usr/share/fonts/TTF/verdana.ttf",
    };
    
    #endif
    
    // 尝试所有可能的字体路径
    for (const auto& path : fontPaths) {
        if (font.openFromFile(path)) {
            std::cout << "Loaded font from: " << path << std::endl;
            return true;
        }
    }
    
    return false;
}




