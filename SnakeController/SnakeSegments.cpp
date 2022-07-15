#include "SnakeSegments.hpp"

void SnakeSegments::pushBackSegment(Segment m_segment){
    m_segments.push_back(m_segment);
}

std::_List_const_iterator<SnakeSegments::Segment> SnakeSegments::getEnd() const {
    return m_segments.end();
}

std::_List_const_iterator<SnakeSegments::Segment> SnakeSegments::getCBegin() const {
    return m_segments.cbegin();
}
std::_List_const_iterator<SnakeSegments::Segment> SnakeSegments::getCEnd() const {
    return m_segments.cend();
}

SnakeSegments::Segment SnakeSegments::getFront() const{
    return m_segments.front();
}

SnakeSegments::Segment SnakeSegments::getBack() const{
    return m_segments.back();
}

void SnakeSegments::popBackSegment(){
    m_segments.pop_back();
}

void SnakeSegments::pushFrontSegment(Segment m_segment){
    m_segments.push_front(m_segment);
}

void SnakeSegments::setDirection(Snake::Direction newDirection){
    m_currentDirection = newDirection;
}

Snake::Direction SnakeSegments::getDirection() const{
    return m_currentDirection;
}

std::size_t SnakeSegments::getSize() const {
    return m_segments.size();
}