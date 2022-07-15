#include "SnakeController.hpp"

#include <algorithm>
#include <sstream>

#include "EventT.hpp"
#include "IPort.hpp"

namespace Snake
{
ConfigurationError::ConfigurationError()
    : std::logic_error("Bad configuration of Snake::Controller.")
{}

UnexpectedEventException::UnexpectedEventException()
    : std::runtime_error("Unexpected event received!")
{}

Controller::Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config)
    : m_displayPort(p_displayPort),
      m_foodPort(p_foodPort),
      m_scorePort(p_scorePort),
      m_paused(false)
{
    std::istringstream istr(p_config);
    char w, f, s, d;

    int width, height, length;
    int foodX, foodY;
    istr >> w >> width >> height >> f >> foodX >> foodY >> s;

    if (w == 'W' and f == 'F' and s == 'S') {
        //m_mapDimension = std::make_pair(width, height);
        //m_foodPosition = std::make_pair(foodX, foodY);
        m_snakeWorld.setMapDimensions(width, height);
        m_snakeWorld.setFoodDimensions(foodX, foodY);

        istr >> d;
        switch (d) {
            case 'U':
                //m_currentDirection = Direction_UP;
                m_snakeSegments.setDirection(Direction_UP);
                break;
            case 'D':
                m_snakeSegments.setDirection(Direction_DOWN);
                break;
            case 'L':
                m_snakeSegments.setDirection(Direction_LEFT);
                break;
            case 'R':
                m_snakeSegments.setDirection(Direction_RIGHT);
                break;
            default:
                throw ConfigurationError();
        }
        istr >> length;

        while (length--) {
            SnakeSegments::Segment seg;
            istr >> seg.x >> seg.y;
            //m_segments.push_back(seg);
            m_snakeSegments.pushBackSegment(seg);
        }
    } else {
        throw ConfigurationError();
    }
}

bool Controller::isSegmentAtPosition(int x, int y) const
{
    /*return m_segments.end() !=  std::find_if(m_segments.cbegin(), m_segments.cend(),
        [x, y](auto const& segment){ return segment.x == x and segment.y == y; });*/
    return m_snakeSegments.getEnd() !=  std::find_if(m_snakeSegments.getCBegin(), m_snakeSegments.getCEnd(),
        [x, y](auto const& segment){ return segment.x == x and segment.y == y; });
}

bool Controller::isPositionOutsideMap(int x, int y) const
{
    //return x < 0 or y < 0 or x >= m_mapDimension.first or y >= m_mapDimension.second;
    return x < 0 or y < 0 or x >= m_snakeWorld.getMapDimensions().first or y >= m_snakeWorld.getMapDimensions().second;
}

void Controller::sendPlaceNewFood(int x, int y)
{
    //m_foodPosition = std::make_pair(x, y);
    m_snakeWorld.setFoodDimensions(x, y);

    DisplayInd placeNewFood;
    placeNewFood.x = x;
    placeNewFood.y = y;
    placeNewFood.value = Cell_FOOD;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(placeNewFood));
}

void Controller::sendClearOldFood()
{
    DisplayInd clearOldFood;
    //clearOldFood.x = m_foodPosition.first;
    //clearOldFood.y = m_foodPosition.second;
    clearOldFood.x = m_snakeWorld.getFoodDimensions().first;
    clearOldFood.y = m_snakeWorld.getFoodDimensions().second;
    clearOldFood.value = Cell_FREE;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(clearOldFood));
}

namespace
{
bool isHorizontal(Direction direction)
{
    return Direction_LEFT == direction or Direction_RIGHT == direction;
}

bool isVertical(Direction direction)
{
    return Direction_UP == direction or Direction_DOWN == direction;
}

bool isPositive(Direction direction)
{
    return (isVertical(direction) and Direction_DOWN == direction)
        or (isHorizontal(direction) and Direction_RIGHT == direction);
}

bool perpendicular(Direction dir1, Direction dir2)
{
    return isHorizontal(dir1) == isVertical(dir2);
}
} // namespace

SnakeSegments::Segment Controller::calculateNewHead() const
{
    //SnakeSegments::Segment const& currentHead = m_segments.front();
    SnakeSegments::Segment const& currentHead = m_snakeSegments.getFront();

    SnakeSegments::Segment newHead;
    newHead.x = currentHead.x + (isHorizontal(m_snakeSegments.getDirection()) ? isPositive(m_snakeSegments.getDirection()) ? 1 : -1 : 0);
    newHead.y = currentHead.y + (isVertical(m_snakeSegments.getDirection()) ? isPositive(m_snakeSegments.getDirection()) ? 1 : -1 : 0);

    return newHead;
}

void Controller::removeTailSegment()
{
    //auto tail = m_segments.back();
    auto tail = m_snakeSegments.getBack();

    DisplayInd l_evt;
    l_evt.x = tail.x;
    l_evt.y = tail.y;
    l_evt.value = Cell_FREE;
    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(l_evt));

    //m_segments.pop_back();
    m_snakeSegments.popBackSegment();
}

void Controller::addHeadSegment(SnakeSegments::Segment const& newHead)
{
    //m_segments.push_front(newHead);
    m_snakeSegments.pushFrontSegment(newHead);

    DisplayInd placeNewHead;
    placeNewHead.x = newHead.x;
    placeNewHead.y = newHead.y;
    placeNewHead.value = Cell_SNAKE;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(placeNewHead));
}

void Controller::removeTailSegmentIfNotScored(SnakeSegments::Segment const& newHead)
{
    if (std::make_pair(newHead.x, newHead.y) == m_snakeWorld.getFoodDimensions()) {
        m_scorePort.send(std::make_unique<EventT<ScoreInd>>());
        m_foodPort.send(std::make_unique<EventT<FoodReq>>());
    } else {
        removeTailSegment();
    }
}

void Controller::updateSegmentsIfSuccessfullMove(SnakeSegments::Segment const& newHead)
{
    if (isSegmentAtPosition(newHead.x, newHead.y) or isPositionOutsideMap(newHead.x, newHead.y)) {
        m_scorePort.send(std::make_unique<EventT<LooseInd>>());
    } else {
        addHeadSegment(newHead);
        removeTailSegmentIfNotScored(newHead);
    }
}

void Controller::handleTimeoutInd()
{
    updateSegmentsIfSuccessfullMove(calculateNewHead());
}

void Controller::handleDirectionInd(std::unique_ptr<Event> e)
{
    auto direction = payload<DirectionInd>(*e).direction;

    if (perpendicular(m_snakeSegments.getDirection(), direction)) {
        m_snakeSegments.setDirection(direction);
    }
}

void Controller::updateFoodPosition(int x, int y, std::function<void()> clearPolicy)
{
    if (isSegmentAtPosition(x, y) || isPositionOutsideMap(x,y)) {
        m_foodPort.send(std::make_unique<EventT<FoodReq>>());
        return;
    }

    clearPolicy();
    sendPlaceNewFood(x, y);
}

void Controller::handleFoodInd(std::unique_ptr<Event> e)
{
    auto receivedFood = payload<FoodInd>(*e);

    updateFoodPosition(receivedFood.x, receivedFood.y, std::bind(&Controller::sendClearOldFood, this));
}

void Controller::handleFoodResp(std::unique_ptr<Event> e)
{
    auto requestedFood = payload<FoodResp>(*e);

    updateFoodPosition(requestedFood.x, requestedFood.y, []{});
}

void Controller::handlePauseInd(std::unique_ptr<Event> e)
{
    m_paused = not m_paused;
}

void Controller::updateScore(int score){
    score = m_snakeSegments.getSize();
    m_scorePort.send(std::make_unique<EventT<ScoreInd>>());
}

void Controller::handleScoreInd(std::unique_ptr<Event> e){
    auto score = payload<ScoreInd>(*e).score;

    updateScore(score);
}

void Controller::receive(std::unique_ptr<Event> e)
{
    switch (e->getMessageId()) {
        case TimeoutInd::MESSAGE_ID:
            if (!m_paused) {
                return handleTimeoutInd();
            }
            return;
        case DirectionInd::MESSAGE_ID:
            if (!m_paused) {
                return handleDirectionInd(std::move(e));
            }
            return;
        case FoodInd::MESSAGE_ID:
            return handleFoodInd(std::move(e));
        case FoodResp::MESSAGE_ID:
            return handleFoodResp(std::move(e));
        case PauseInd::MESSAGE_ID:
            return handlePauseInd(std::move(e));
        case ScoreInd::MESSAGE_ID:
            return handleScoreInd(std::move(e));
        default:
            throw UnexpectedEventException();
    }
}

} // namespace Snake
