#include "SnakeInterface.hpp"
#include <list>

class SnakeSegments{
public:
    struct Segment
    {
        int x;
        int y;
    };

    SnakeSegments::Segment getFront() const;
    SnakeSegments::Segment getBack() const;

    std::_List_const_iterator<Segment> getEnd() const;
    std::_List_const_iterator<Segment> getCBegin() const;
    std::_List_const_iterator<Segment> getCEnd() const; 

    void pushBackSegment(Segment m_segment);
    void popBackSegment();
    void pushFrontSegment(Segment m_segment);
    void setDirection(Snake::Direction newDirection);
    
    Snake::Direction getDirection() const;

private:    
    std::list<Segment> m_segments;
    Snake::Direction m_currentDirection;
};