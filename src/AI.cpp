#include "AI.hpp"
#include <iostream>
#include <algorithm>

inline sf::Vector2i absVector2i(const sf::Vector2i& v) {
    sf::Vector2i t;
    t.x = std::abs(v.x);
    t.y = std::abs(v.y);
    return t;
}


void decideNextMove(std::queue<sf::Vector2i>& action, const World& world, const Snake& snake) {

    auto path = aStarSearch(snake, snake.node[0], world.pellet);
    /*auto current = snake.node[0];
    for (const auto& next : path) {
        action.push(next - current);
        current = next;
    }*/
    // 找到路径，只取第一步
    //std::cout << "path : (" << path[0].x << " " <<  path[0].y << ")\n";
    if (!path.empty()) {
        sf::Vector2i nextStep = path[0];
        sf::Vector2i current = snake.node[0];
        action.push(nextStep - current);
    } else {
        // 此时如果不做处理，蛇就会撞死。
        // 简单的兜底：随便找一个没被堵住的邻居格子走（为了活命）
        // 更好的兜底（之前的教程提到的）：去追自己的尾巴 (snake.node.back())
        
        auto pathToTail = aStarSearch(snake, snake.node[0], snake.node.back());
        if (!pathToTail.empty()) {
             sf::Vector2i nextStep = pathToTail[0];
             action.push(nextStep - snake.node[0]);
        } else {
            // 实在没办法了，找个合法的格子随便走一步，尽量不要立即死
            // (代码略：遍历上下左右，找一个不是墙也不是蛇身的格子)
            std::vector<sf::Vector2i> dir = {
                {1, 0},
                {-1, 0},
                {0, 1},
                {0, -1}
            };
            for (const auto& d : dir) {
                auto newPath = snake.node[0] + d;
                if (std::find(snake.node.begin(), snake.node.end(), newPath) == snake.node.end()) {
                    action.push(d);
                    break;
                }
            }
        }
    }


}


std::vector<sf::Vector2i> aStarSearch(const Snake& snake, const sf::Vector2i& begin, const sf::Vector2i& end) {
    
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> frontier;

    frontier.push({begin, 0, std::abs(end.x - begin.x) + std::abs(end.y - begin.y), {-1, -1}});

    std::vector<sf::Vector2i> obstacle(snake.node.begin() , snake.node.end());

    std::vector<std::vector<bool>> isarrive(ROW, std::vector<bool>(COL, false));

    std::vector<std::vector<sf::Vector2i>> parent(ROW, std::vector<sf::Vector2i>(COL)); 

    isarrive[begin.x][begin.y] = true;
    parent[begin.x][begin.y] = {-1, -1}; 
    std::vector<sf::Vector2i> dir = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    };

    double cost = 1;

    std::vector<sf::Vector2i> path;

    while (!frontier.empty()) {

        auto current = frontier.top();
        frontier.pop();
        // 如果是目标节点
        if (current.p == end) {
            // 回溯路线
            
            auto cur = end;
            while (cur != (sf::Vector2i){-1, -1}) {
                path.push_back(cur);
                cur = parent[cur.x][cur.y];
            }
            std::reverse(path.begin(), path.end());
            break;    
        }

        for (const auto& d : dir) {
            int nx = current.p.x + d.x;
            int ny = current.p.y + d.y;

            sf::Vector2i npos = {nx ,ny};

            // 边界情况
            if (ny < 0 || ny >= COL || nx < 0 || nx >= ROW ) {
                continue;
            }

            if(isarrive[nx][ny]) {
                continue;
            }

            auto it = std::find(obstacle.begin(), obstacle.end(), npos);

            if (it != obstacle.end()) {
                continue;
            }

            auto [hx, hy] = absVector2i(npos - end);

            AStarNode newNode(npos, current.g + cost, hx + hy, current.p);
            isarrive[nx][ny] = true;
            parent[nx][ny] = current.p;
            frontier.push(newNode);

        }


    }

    if (path.size() > 1) {
        path.erase(path.begin());
    } else if (!path.empty()) {
        path.clear();
    }

    return path;

}