#include <memory>

class SnakeWorld {
private:
    std::pair<int, int> m_mapDimension;
    std::pair<int, int> m_foodPosition;
public:
    void setMapDimensions(int width, int height);
    void setFoodDimensions(int foodX, int foodY);
    std::pair<int, int> getFoodDimensions() const;
    std::pair<int, int> getMapDimensions() const;
};