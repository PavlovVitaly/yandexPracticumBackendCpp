#pragma once
#include "tagged.h"

#include <string>
#include <unordered_map>

namespace model {

enum class Direction {
    NORTH,
    SOUTH,
    WEST,
    EAST
};

const std::unordered_map<Direction, std::string> DIRECTION_TO_STRING = {
    {Direction::NORTH, "U"},
    {Direction::SOUTH, "D"},
    {Direction::WEST,  "L"},
    {Direction::EAST,  "R"}
};

const std::unordered_map<std::string, Direction> STRING_TO_DIRECTION = {
    {"U", Direction::NORTH},
    {"D", Direction::SOUTH},
    {"L", Direction::WEST},
    {"R", Direction::EAST}
};

struct Position {
    double x, y;
};

struct Velocity {
    double vx, vy;
};

class Dog {
    inline static size_t max_id_cont_ = 0;
public:
    using Id = util::Tagged<size_t, Dog>;
    Dog(std::string name) : 
        id_(Id{Dog::max_id_cont_++}),
        name_(name) {};
    Dog(Id id, std::string name) :
        id_(id),
        name_(name) {};
    Dog(const Dog& other) = default;
    Dog(Dog&& other) = default;
    Dog& operator = (const Dog& other) = default;
    Dog& operator = (Dog&& other) = default;
    virtual ~Dog() = default;

    const Id& GetId() const;
    const std::string& GetName() const;

    void SetDirection(Direction direction);
    const Direction GetDirection() const;

    void SetPosition(Position position);
    const Position& GetPosition() const;

    void SetVelocity(Velocity velocity);
    const Velocity& GetVelocity() const;
    
private:
    Id id_;
    std::string name_;
    Direction direction_{Direction::NORTH};
    Position position_{0.0, 0.0};
    Velocity velocity_{0.0, 0.0};
};

}