#include "SnakeWorld.hpp"
#include <memory>

void SnakeWorld::setFoodDimensions(int foodX, int foodY){
    m_foodPosition = std::make_pair(foodX, foodY);
}

std::pair<int, int> SnakeWorld::getFoodDimensions() const{
    return m_foodPosition;
}

void SnakeWorld::setMapDimensions(int width, int height){
    m_mapDimension = std::make_pair(width, height);
}

std::pair<int, int> SnakeWorld::getMapDimensions() const{
    return m_mapDimension;
}