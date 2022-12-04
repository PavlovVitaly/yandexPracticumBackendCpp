#include "roadmap.h"

#include <cmath>

#include <iostream>
#include <stdint.h>

namespace model {

const double OFFSET = 0.4;

Roadmap::Roadmap(const Roadmap& other) {
    CopyContent(other.roads_);
};

Roadmap::Roadmap(Roadmap&& other) {
    matrix_map_ = std::move(other.matrix_map_);
    roads_ = std::move(other.roads_);
};

Roadmap& Roadmap::operator = (const Roadmap& other) {
    if(this != &other) {
        CopyContent(other.roads_);
    }
    return *this;
};

Roadmap& Roadmap::operator = (Roadmap&& other) {
    if(this != &other) {
        matrix_map_ = std::move(other.matrix_map_);
        roads_ = std::move(other.roads_);
    }
    return *this;
};

void Roadmap::AddRoad(const Road& road) {
    size_t index = roads_.size();
    roads_.emplace_back(road);
    if(road.IsHorizontal()) {
        int step = (road.GetStart().x < road.GetEnd().x) ? 1 : -1;
        for(int x = road.GetStart().x; x <= road.GetEnd().x; x += step) {
            matrix_map_[x][road.GetStart().y].insert(index);
            matrix_map_[x][road.GetStart().y + 1].insert(index);
        }
    } else {
        int step = (road.GetStart().y < road.GetEnd().y) ? 1 : -1;
        for(int y = road.GetStart().y; y <= road.GetEnd().y; y += step) {
            matrix_map_[road.GetStart().x][y].insert(index);
            matrix_map_[road.GetStart().x + 1][y].insert(index);
        }
    }
};

const Roadmap::Roads& Roadmap::GetRoads() const noexcept {
    return roads_;
};

std::tuple<Position, Velocity> Roadmap::GetValidMove(const Position& old_position,
                            const Position& potential_new_position,
                            const Velocity& old_velocity) {
    Velocity velocity = old_velocity;
    auto start_roads = GetCoordinatesOfPosition(old_position);
    auto end_roads = GetCoordinatesOfPosition(potential_new_position);
    if(end_roads){
        if(!IsValidPosition(matrix_map_[end_roads.value().x][end_roads.value().y], potential_new_position)) {
            end_roads = std::nullopt;
            velocity = {0, 0};
        } else if(start_roads == end_roads) {
            return std::tie(potential_new_position, velocity);
        }
    } else {
        velocity = {0, 0};
    }
    auto dest = GetDestinationRoadsOfRoute(start_roads, end_roads, old_velocity);
    Position position = (dest && IsValidPosition(dest.value(), potential_new_position)) ?
            potential_new_position:
            GetFarestpoinOfRoute(dest.value(), potential_new_position, old_velocity);
    return std::tie(position, velocity);
};

std::optional<const std::unordered_set<size_t>> Roadmap::GetDestinationRoadsOfRoute(std::optional<const MatrixMapCoord> start,
                                    std::optional<const MatrixMapCoord> end,
                                    const Velocity& old_velocity) {
    const MatrixMapCoord start_coord = start.value();
    std::unordered_set<size_t> current_roads;
    if(old_velocity.vx != 0) {
        int direction = std::signbit(old_velocity.vx) ? -1 : 1;
        int64_t end_x{0};
        if(end) {
            end_x = (direction > 0) ? (end.value().x < LLONG_MAX ? end.value().x + 1 : LLONG_MAX) :
                                        end.value().x - 1;
        } else {
            end_x = (direction > 0) ? LLONG_MAX : -1;
        }
        int64_t ind{0};
        for(ind = start_coord.x; ind != end_x; ind += direction) {
            size_t index = static_cast<size_t>(ind);
            if(ValidateCoordinates({index, start_coord.y}) &&
                IsCrossedSets(matrix_map_[start_coord.x][start_coord.y],
                                matrix_map_[index][start_coord.y])) {
                current_roads =  matrix_map_[index][start_coord.y];
            } else {
                break;
            }
        }
        if(current_roads.empty()) {
            return std::nullopt; 
        }
        return current_roads;
    } else if(old_velocity.vy != 0) {
        int direction = std::signbit(old_velocity.vy) ? -1 : 1;
        int64_t end_y{0};
        if(end) {
            end_y = (direction > 0) ? (end.value().y < LLONG_MAX ? end.value().y + 1 : LLONG_MAX) :
                                        end.value().y - 1;
        } else {
            end_y = (direction > 0) ? LLONG_MAX : -1;
        }
        int64_t ind{0};
        for(ind = start_coord.y; ind != end_y; ind += direction) {
            size_t index = static_cast<size_t>(ind);
            if(ValidateCoordinates({start_coord.x, index}) &&
                IsCrossedSets(matrix_map_[start_coord.x][start_coord.y],
                                matrix_map_[start_coord.x][index])) {
                current_roads =  matrix_map_[start_coord.x][index];
            } else {
                break;
            }
        }
        if(current_roads.empty()) {
            return std::nullopt; 
        }
        return current_roads;
    }
    return std::nullopt;
};

std::optional<const Roadmap::MatrixMapCoord> Roadmap::GetCoordinatesOfPosition(const Position& position) {
    if(position.x < -OFFSET || position.y < -OFFSET) {
        return std::nullopt;
    }
    size_t x_index = std::floor(std::abs(position.x));
    size_t y_index = std::floor(std::abs(position.y));
    if(matrix_map_.contains(x_index)) {
        if(matrix_map_[x_index].contains(y_index)) {
            return MatrixMapCoord{x_index, y_index};
        }
    }
    return std::nullopt;
};

bool Roadmap::IsCrossedSets(const std::unordered_set<size_t>& lhs,
                            const std::unordered_set<size_t>& rhs) {
    for(auto item : lhs) {
        if(rhs.contains(item)) {
            return true;
        }
    }
    return false;
};

bool Roadmap::ValidateCoordinates(const MatrixMapCoord& coordinates) {
    if(matrix_map_.contains(coordinates.x)) {
        return matrix_map_[coordinates.x].contains(coordinates.y);
    }
    return false;
};

const Position Roadmap::GetFarestpoinOfRoute(const std::unordered_set<size_t>& roads,
                                    const Position invalid_position,
                                    const Velocity& old_velocity) {
    Position res_position = invalid_position;
    for(auto road_ind : roads) {
        auto start = (roads_[road_ind].GetEnd().x - roads_[road_ind].GetStart().x >= 0) &&
                    (roads_[road_ind].GetEnd().y - roads_[road_ind].GetStart().y >= 0) ?
                    roads_[road_ind].GetStart() :
                    roads_[road_ind].GetEnd();
        auto end = (roads_[road_ind].GetEnd().x - roads_[road_ind].GetStart().x >= 0) &&
                    (roads_[road_ind].GetEnd().y - roads_[road_ind].GetStart().y >= 0) ?
                    roads_[road_ind].GetEnd() :
                    roads_[road_ind].GetStart();
        if(old_velocity.vx < 0) {
            res_position.x = start.x - OFFSET;
        } else if(old_velocity.vx > 0) {
            res_position.x = end.x + OFFSET;
        } else if(old_velocity.vy < 0) {
            res_position.y = start.y - OFFSET;
        } else if(old_velocity.vy > 0) {
            res_position.y = end.y + OFFSET;
        }
        break;  // todo: rework. temp changes.
    }
    return res_position;
};

bool Roadmap::IsValidPosition(const std::unordered_set<size_t>& roads_ind, const Position& position) {
    for(auto road_index : roads_ind) {
        if(IsValidPositionOnRoad(roads_[road_index], position)) {
            return true;
        }
    }
    return false;
};

bool Roadmap::IsValidPositionOnRoad(const Road& road, const Position& position) {
    double start_x, end_x, start_y, end_y;
    if(road.IsHorizontal()) {
        start_x = (road.GetStart().x < road.GetEnd().x) ? (road.GetStart().x) : (road.GetEnd().x);
        end_x = (road.GetStart().x < road.GetEnd().x) ? (road.GetEnd().x) : (road.GetStart().x);
        start_y = road.GetStart().y - OFFSET;
        end_y = road.GetStart().y + OFFSET;

        start_x -= OFFSET;
        end_x += OFFSET;
    } else {
        start_y = (road.GetStart().y < road.GetEnd().y) ? (road.GetStart().y) : (road.GetEnd().y);
        end_y = (road.GetStart().y < road.GetEnd().y) ? (road.GetEnd().y) : (road.GetStart().y);
        start_x = road.GetStart().x - OFFSET;
        end_x = road.GetStart().x + OFFSET;

        start_y -= OFFSET;
        end_y += OFFSET;
    }
    return (position.x >= start_x && position.x <= end_x) &&
            (position.y >= start_y && position.y <= end_y);
};

void Roadmap::CopyContent(const Roadmap::Roads& roads) {
    for(auto& road : roads) {
        AddRoad(road);
    }
};

}