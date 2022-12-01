#include "roadmap.h"

#include <cmath>

#include <iostream>

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

bool Roadmap::IsValidPosition(const Position& position) {
    size_t x_index = std::floor(position.x);
    size_t y_index = std::floor(position.y);
    if(matrix_map_.contains(x_index)) {
        if(matrix_map_[x_index].contains(y_index)) {
            for(auto road_index : matrix_map_[x_index][y_index]) {
                if(IsValidPositionOnRoad(roads_[road_index], position)) {
                    return true;
                }
            }
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