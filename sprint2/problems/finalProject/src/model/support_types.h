#pragma once

#include <string>
#include <unordered_map>

namespace model {

using Dimension = int;
using Coord = Dimension;

const double MILLISECONDS_IN_SECOND = 1000.0;
const double OFFSET = 0.4;

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

struct Position {
    double x, y;
};

struct Velocity {
    double vx, vy;
};

}