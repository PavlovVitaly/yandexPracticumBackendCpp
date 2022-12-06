#pragma once
#include "road.h"
#include "support_types.h"

#include <map>
#include <unordered_set>
#include <tuple>
#include <optional>

namespace model {

/*Класс для предстваления карты дорог. Map разбивается на ячейки с целочисленными координатами
в каждой ячейке хранится множество дорог проходящих по данному участку*/
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
    std::tuple<Position, Velocity> GetValidMove(const Position& old_position,
                            const Position& potential_new_position,
                            const Velocity& old_velocity);
private:
    struct MatrixMapCoord {
        int64_t x;
        int64_t y;

        bool operator == (const MatrixMapCoord& other) const {
            return (x == other.x) && (y == other.y); 
        };
    };
    using MatrixMap = std::map< int64_t, std::map<int64_t, std::unordered_set<size_t> > >;
    MatrixMap matrix_map_;
    Roads roads_;

    std::optional<const std::unordered_set<size_t>> GetDestinationRoadsOfRoute(std::optional<const MatrixMapCoord> start,
                                    std::optional<const MatrixMapCoord> end,
                                    const Velocity& old_velocity);
    std::optional<const MatrixMapCoord> GetCoordinatesOfPosition(const Position& position);
    bool IsCrossedSets(const std::unordered_set<size_t>& lhs,
                        const std::unordered_set<size_t>& rhs);
    bool ValidateCoordinates(const MatrixMapCoord& coordinates);
    const Position GetFarestPoinOfRoute(const std::unordered_set<size_t>& roads,
                                    const Position invalid_position,
                                    const Velocity& old_velocity);
    bool IsValidPosition(const std::unordered_set<size_t>& roads_ind,
                        const Position& position);
    
    bool IsValidPositionOnRoad(const Road& road, const Position& position);
    void CopyContent(const Roads& roads);
};

}