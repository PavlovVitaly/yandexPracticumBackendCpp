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
    std::tuple<geom::Point2D, Velocity> GetValidMove(const geom::Point2D& old_position,
                            const geom::Point2D& potential_new_position,
                            const Velocity& old_velocity) const;
    geom::Point2D GenerateValidRandomPosition() const;
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

    std::optional<const MatrixMapCoord> GetDestinationRoadsOfRoute(
        std::optional<const MatrixMapCoord> start,
        std::optional<const MatrixMapCoord> end,
        const Velocity& old_velocity) const;
    std::optional<const MatrixMapCoord> GetCoordinatesOfPosition(const geom::Point2D& position) const;
    const Direction VelocityToDirection(const Velocity& velocity) const;
    const std::unordered_map<Direction, geom::Point2D> MatrixCoordinateToPosition(
        const MatrixMapCoord& coord, const geom::Point2D& target_position) const;
    bool IsCrossedSets(const std::unordered_set<size_t>& lhs,
                        const std::unordered_set<size_t>& rhs) const;
    bool ValidateCoordinates(const MatrixMapCoord& coordinates) const;
    const geom::Point2D GetFarestPoinOfRoute(const MatrixMapCoord& roads_coord,
                                        const geom::Point2D& old_position,
                                        const Velocity& old_velocity) const;
    bool IsValidPosition(const std::unordered_set<size_t>& roads_ind,
                        const geom::Point2D& position) const;
    
    bool IsValidPositionOnRoad(const Road& road, const geom::Point2D& position) const;
    void CopyContent(const Roads& roads);
};

}