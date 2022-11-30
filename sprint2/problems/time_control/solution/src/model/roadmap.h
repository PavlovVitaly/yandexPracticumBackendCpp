#pragma once
#include "road.h"
#include "support_types.h"

#include <unordered_map>
#include <unordered_set>
#include <tuple>

namespace model {

class Roadmap {
public:
    using Roads = std::vector<Road>;

    Roadmap(Roads& roads);
    Roadmap(const Roadmap& other) = delete;
    Roadmap(Roadmap&& other) = delete;
    Roadmap& operator = (const Roadmap& other) = delete;
    Roadmap& operator = (Roadmap&& other) = delete;
    virtual ~Roadmap() = default;

    bool IsValidPosition(const Position& position);
private:
    using MatrixMap = std::unordered_map< size_t, std::unordered_map<size_t, std::unordered_set<size_t> > >;
    MatrixMap matrix_map_;
    Roads roads_;

    bool IsValidPositionOnRoad(const Road& road, const Position& position);
};

}