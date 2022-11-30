#include "roadmap.h"

#include <cmath>

#include <iostream>

namespace model {

const double OFFSET = 0.4;

Roadmap::Roadmap(Roads& roads): roads_(roads) {
    for(size_t i = 0; i < roads.size(); ++i) {
        auto& road = roads[i];
        if(road.IsHorizontal()) {
            int step = (road.GetStart().x < road.GetEnd().x) ? 1 : -1;
            for(int x = road.GetStart().x; x <= road.GetEnd().x; x += step) {
                matrix_map_[x][road.GetStart().y].insert(i);
                matrix_map_[x][road.GetStart().y + 1].insert(i);
            }
        } else {
            int step = (road.GetStart().y < road.GetEnd().y) ? 1 : -1;
            for(int y = road.GetStart().y; y <= road.GetEnd().y; y += step) {
                matrix_map_[road.GetStart().x][y].insert(i);
                matrix_map_[road.GetStart().x + 1][y].insert(i);
            }
        }
    }
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
        start_x = (road.GetStart().x < road.GetEnd().x) ? (road.GetStart().x + OFFSET) : (road.GetEnd().x - OFFSET);
        end_x = (road.GetStart().x < road.GetEnd().x) ? (road.GetEnd().x - OFFSET) : (road.GetStart().x + OFFSET);
        start_y = road.GetStart().y - OFFSET;
        end_y = road.GetStart().y + OFFSET;

        start_y = (start_y < 0) ? 0 : start_y;
        end_y = (end_y < 0) ? 0 : end_y;
    } else {
        start_y = (road.GetStart().y < road.GetEnd().y) ? (road.GetStart().y + OFFSET) : (road.GetEnd().y - OFFSET);
        end_y = (road.GetStart().y < road.GetEnd().y) ? (road.GetEnd().y - OFFSET) : (road.GetStart().y + OFFSET);
        start_x = road.GetStart().x - OFFSET;
        end_x = road.GetStart().x + OFFSET;

        start_x = (start_x < 0) ? 0 : start_x;
        end_x = (end_x < 0) ? 0 : end_x;
    }
    return (position.x >= start_x && position.x <= end_x) &&
            (position.y >= start_y && position.y <= end_y);
};

}