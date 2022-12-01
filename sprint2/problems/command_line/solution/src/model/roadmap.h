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

    Roadmap() = default;
    Roadmap(const Roadmap& other);
    Roadmap(Roadmap&& other);
    Roadmap& operator = (const Roadmap& other);
    Roadmap& operator = (Roadmap&& other);
    virtual ~Roadmap() = default;

    void AddRoad(const Road& road);
    const Roads& GetRoads() const noexcept;
    bool IsValidPosition(const Position& position);
private:
    using MatrixMap = std::unordered_map< size_t, std::unordered_map<size_t, std::unordered_set<size_t> > >;
    MatrixMap matrix_map_;
    Roads roads_;

    bool IsValidPositionOnRoad(const Road& road, const Position& position);
    void CopyContent(const Roads& roads);
};

}