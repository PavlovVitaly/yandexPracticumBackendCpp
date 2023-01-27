#pragma once

#include <string>
#include <unordered_map>

namespace model {

using Dimension = int;
using Coord = Dimension;

struct Point {
    Coord x, y;
};

struct Size {
    Dimension width, height;
};

struct Rectangle {
    Point position;
    Size size;
};

struct Offset {
    Dimension dx, dy;
};

enum class Direction {
    NORTH,
    SOUTH,
    WEST,
    EAST,
    NONE
};

const std::unordered_map<Direction, std::string> DIRECTION_TO_STRING = {
    {Direction::NORTH, "U"},
    {Direction::SOUTH, "D"},
    {Direction::WEST,  "L"},
    {Direction::EAST,  "R"},
    {Direction::NONE,  ""}
};

const std::unordered_map<std::string, Direction> STRING_TO_DIRECTION = {
    {"U", Direction::NORTH},
    {"D", Direction::SOUTH},
    {"L", Direction::WEST},
    {"R", Direction::EAST},
    {"", Direction::NONE}
};

struct Velocity {
    double vx, vy;
};

bool operator == (const Velocity& lhs, const Velocity& rhs);

struct VelocityHasher {
  size_t operator()(const Velocity& velocity) const{
    size_t sd = 17;
    return std::hash<double>{}(velocity.vy)*sd + std::hash<double>{}(velocity.vx);
  }
};

const std::unordered_map<Velocity, Direction, VelocityHasher> VELOCITY_TO_DIRECTION = {
    {{0, -1}, Direction::NORTH},
    {{0, 1}, Direction::SOUTH},
    {{-1, 0}, Direction::WEST},
    {{1, 0}, Direction::EAST},
    {{0, 0}, Direction::NONE}
};

const std::unordered_map<Direction, Direction> DIRECTION_TO_OPOSITE_DIRECTION = {
    {Direction::NORTH, Direction::SOUTH},
    {Direction::SOUTH, Direction::NORTH},
    {Direction::WEST, Direction::EAST},
    {Direction::EAST, Direction::WEST},
    {Direction::NONE, Direction::NONE}
};

}