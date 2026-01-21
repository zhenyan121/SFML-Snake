#pragma once
#include <queue>
#include "Game.hpp" 

struct AStarNode {
    sf::Vector2i p;
    double f, g, h;
    sf::Vector2i parent;

     // 添加默认构造函数
    AStarNode() : p(-1, -1), g(0.0), h(0.0), parent(-1, -1) {}

    AStarNode(sf::Vector2i p, double g = 0, double h = 0, sf::Vector2i parent = {-1, -1})
        : p(p), g(g), h(h), f(g + h), parent(parent) {}
    
    // 小顶堆
    bool operator>(const AStarNode& node) const{
        return f > node.f;
    }

};

std::vector<sf::Vector2i> aStarSearch(const Snake& snake, const sf::Vector2i& begin, const sf::Vector2i& end);


void decideNextMove(std::queue<sf::Vector2i>& action, const World& world, const Snake& snake);

bool isSafeMove(const Snake& snake, const World& world, const sf::Vector2i& nextStep);
