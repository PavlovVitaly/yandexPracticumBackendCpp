#include "roadmap.h"
#include "random_generators.h"
#include "model_invariants.h"

#include <cmath>
#include <stdint.h>
#include <set>

namespace model {

/*Такой масштаб выбран, чтобы в одной клетке не было нескольких дорог без их наложения друг на друга
(условие непрерывности маршрута если в клетке есть какая-нибудь дорога).*/
const int SCALE_FACTOR_OF_CELL = 20;    // Разбиваем карту на квадраты размером 0.05x0.05 папугаев.

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
    const auto SCALLED_OFFSET = static_cast<int64_t>(OFFSET * SCALE_FACTOR_OF_CELL);
    auto index = roads_.size();
    roads_.emplace_back(road);
    if(road.IsHorizontal()) {
        auto start = static_cast<int64_t>((road.GetStart().x < road.GetEnd().x) ? road.GetStart().x : road.GetEnd().x);
        auto end = static_cast<int64_t>((road.GetStart().x < road.GetEnd().x) ? road.GetEnd().x : road.GetStart().x);
        start = start * SCALE_FACTOR_OF_CELL - SCALLED_OFFSET;
        end = end * SCALE_FACTOR_OF_CELL + SCALLED_OFFSET;
        auto y = road.GetStart().y * SCALE_FACTOR_OF_CELL;
        for(auto x = start; x <= end; ++x) {
            for(auto i = -(SCALLED_OFFSET); i <= SCALLED_OFFSET; ++i) {
                matrix_map_[x][y + i].insert(index);
            }
        }
    } else {
        auto start = static_cast<int64_t>((road.GetStart().y < road.GetEnd().y) ? road.GetStart().y : road.GetEnd().y);
        auto end = static_cast<int64_t>((road.GetStart().y < road.GetEnd().y) ? road.GetEnd().y : road.GetStart().y);
        start = start * SCALE_FACTOR_OF_CELL - SCALLED_OFFSET;
        end = end * SCALE_FACTOR_OF_CELL + SCALLED_OFFSET;
        auto x = road.GetStart().x * SCALE_FACTOR_OF_CELL;
        for(auto y = start; y <= end; ++y) {
            for(auto i = -SCALLED_OFFSET; i <= SCALLED_OFFSET; ++i) {
                matrix_map_[x + i][y].insert(index);
            }
        }
    }
};

const Roadmap::Roads& Roadmap::GetRoads() const noexcept {
    return roads_;
};

std::tuple<geom::Point2D, Velocity> Roadmap::GetValidMove(const geom::Point2D& old_position,
                            const geom::Point2D& potential_new_position,
                            const Velocity& old_velocity) const {
    Velocity velocity = {0, 0};
    auto start_roads = GetCoordinatesOfPosition(old_position);
    auto end_roads = GetCoordinatesOfPosition(potential_new_position);
    if(end_roads){
        if(!IsValidPosition(
                matrix_map_
                .at(end_roads.value().x)
                .at(end_roads.value().y),
                potential_new_position
            )
        ) {
            end_roads = std::nullopt;
        } else if(start_roads == end_roads) {
            return std::tie(potential_new_position, old_velocity);
        }
    }
    auto dest = GetDestinationRoadsOfRoute(start_roads, end_roads, old_velocity);
    geom::Point2D position;
    if(dest 
        && IsValidPosition(
            matrix_map_
            .at(dest.value().x)
            .at(dest.value().y),
            potential_new_position
        )
    ) {
        position = potential_new_position;
        velocity = old_velocity;
    } else {
        position = GetFarestPoinOfRoute(dest.value(), old_position, old_velocity);
    }
    return std::tie(position, velocity);
};

geom::Point2D Roadmap::GenerateValidRandomPosition() const {
    geom::Point2D pos;
    int road_index = utils::GenerateIntegerFromInterval(0, roads_.size() - 1);
    auto road = roads_[road_index];
    if(road.IsHorizontal()) {
        pos.x = utils::GenerateDoubleFromInterval(road.GetStart().x,
                                                road.GetEnd().x);
        pos.y = road.GetStart().y;
    } else {
        pos.y = utils::GenerateDoubleFromInterval(road.GetStart().y,
                                                road.GetEnd().y);
        pos.x = road.GetStart().x;
    }
    return pos;
};

std::optional<const Roadmap::MatrixMapCoord> Roadmap::GetDestinationRoadsOfRoute(
                                    std::optional<const MatrixMapCoord> start,
                                    std::optional<const MatrixMapCoord> end,
                                    const Velocity& old_velocity) const {
    const MatrixMapCoord start_coord = start.value();
    MatrixMapCoord current_coord = start_coord;
    if(old_velocity.vx != 0) {
        int direction = std::signbit(old_velocity.vx) ? -1 : 1;
        int64_t end_x{0};
        if(end) {
            end_x = end.value().x * SCALE_FACTOR_OF_CELL;
            end_x = (direction > 0) ? (end_x < LLONG_MAX ? end_x + 1 : LLONG_MAX) :
                                        end_x - 1;
        } else {
            end_x = (direction > 0) ? LLONG_MAX : -(OFFSET * SCALE_FACTOR_OF_CELL) - 1;
        }
        int64_t index{0};
        for(index = start_coord.x; index != end_x; index += direction) {
            if(ValidateCoordinates({index, start_coord.y}) &&
                IsCrossedSets(
                    matrix_map_
                    .at(start_coord.x)
                    .at(start_coord.y),
                    matrix_map_
                    .at(index)
                    .at(start_coord.y)
                )
            ) {
                current_coord = {index, start_coord.y};
            } else {
                break;
            }
        }
        return current_coord;
    } else if(old_velocity.vy != 0) {
        int direction = std::signbit(old_velocity.vy) ? -1 : 1;
        int64_t end_y{0};
        if(end) {
            end_y = end.value().y * SCALE_FACTOR_OF_CELL;
            end_y = (direction > 0) ? (end_y < LLONG_MAX ? end_y + 1 : LLONG_MAX) :
                                        end_y - 1;
        } else {
            end_y = (direction > 0) ? LLONG_MAX : -(OFFSET * SCALE_FACTOR_OF_CELL)  - 1;
        }
        int64_t index{0};
        for(index = start_coord.y; index != end_y; index += direction) {
            if(ValidateCoordinates({start_coord.x, index}) &&
                IsCrossedSets(
                    matrix_map_
                    .at(start_coord.x)
                    .at(start_coord.y),
                    matrix_map_
                    .at(start_coord.x)
                    .at(index))) {
                current_coord =  {start_coord.x, index};
            } else {
                break;
            }
        }
        return current_coord;
    }
    return std::nullopt;
};

std::optional<const Roadmap::MatrixMapCoord> Roadmap::GetCoordinatesOfPosition(const geom::Point2D& position) const {
    if(position.x < -OFFSET - EPSILON || position.y < -OFFSET - EPSILON) {
        return std::nullopt;
    }
    int64_t x_index = (position.x >= 0) ? std::floor(position.x * SCALE_FACTOR_OF_CELL) : std::ceil(position.x * SCALE_FACTOR_OF_CELL);
    int64_t y_index = (position.y >= 0) ? std::floor(position.y * SCALE_FACTOR_OF_CELL) : std::ceil(position.y * SCALE_FACTOR_OF_CELL);
    if(matrix_map_.contains(x_index)) {
        if(matrix_map_
            .at(x_index)
            .contains(y_index)) {
            return MatrixMapCoord{x_index, y_index};
        }
    }
    return std::nullopt;
};

bool Roadmap::IsCrossedSets(const std::unordered_set<size_t>& lhs,
                            const std::unordered_set<size_t>& rhs) const {
    for(auto item : lhs) {
        if(rhs.contains(item)) {
            return true;
        }
    }
    return false;
};

bool Roadmap::ValidateCoordinates(const MatrixMapCoord& coordinates) const{
    if(matrix_map_.contains(coordinates.x)) {
        return matrix_map_
                .at(coordinates.x)
                .contains(coordinates.y);
    }
    return false;
};

const geom::Point2D Roadmap::GetFarestPoinOfRoute(const MatrixMapCoord& roads_coord,
                                    const geom::Point2D& old_position,
                                    const Velocity& old_velocity) const {
    geom::Point2D res_position{old_position};
    auto cell_pos = MatrixCoordinateToPosition(roads_coord, old_position);
    auto direction = VelocityToDirection(old_velocity);
    for(auto road_ind : matrix_map_
                        .at(roads_coord.x)
                        .at(roads_coord.y)
    ) {
        auto start_position = cell_pos.at(DIRECTION_TO_OPOSITE_DIRECTION.at(direction));
        auto end_position = cell_pos.at(direction);
        if(IsValidPositionOnRoad(roads_[road_ind], start_position)) {
            if(IsValidPositionOnRoad(roads_[road_ind], end_position)) {
                return end_position;
            }
            res_position = start_position;
        }
    }
    return res_position;
};

const std::unordered_map<Direction, geom::Point2D> Roadmap::MatrixCoordinateToPosition(
    const MatrixMapCoord& coord, const geom::Point2D& target_position) const {
    std::unordered_map<Direction, geom::Point2D> res;
    int64_t x_inc_e = (coord.x < 0) ? 0 : 1;
    int64_t y_inc_s = (coord.y < 0) ? 0 : 1;
    int64_t x_inc_w = (coord.x < 0) ? -1 : 0;
    int64_t y_inc_n = (coord.y < 0) ? -1 : 0;
    res[Direction::NORTH] = geom::Point2D{
        target_position.x,
        (static_cast<double>(coord.y + y_inc_n) / static_cast<double>(SCALE_FACTOR_OF_CELL))};
    res[Direction::SOUTH] = geom::Point2D{
        target_position.x,
        (static_cast<double>(coord.y + y_inc_s) / static_cast<double>(SCALE_FACTOR_OF_CELL))};
    res[Direction::WEST] = geom::Point2D{
        (static_cast<double>(coord.x + x_inc_w) / static_cast<double>(SCALE_FACTOR_OF_CELL)),
        target_position.y};
    res[Direction::EAST] = geom::Point2D{
        (static_cast<double>(coord.x + x_inc_e) / static_cast<double>(SCALE_FACTOR_OF_CELL)),
        target_position.y};
    res[Direction::NONE] = geom::Point2D{target_position.x, target_position.y};
    return res;
}

const Direction Roadmap::VelocityToDirection(const Velocity& velocity) const {
    Velocity vel{0, 0};
    if(velocity.vx != 0) {
        vel.vx = std::signbit(velocity.vx) ? -1 : 1;
    }
    if(velocity.vy != 0) {
        vel.vy = std::signbit(velocity.vy) ? -1 : 1;
    }
    return VELOCITY_TO_DIRECTION.at(vel);
}

bool Roadmap::IsValidPosition(const std::unordered_set<size_t>& roads_ind,
                                const geom::Point2D& position) const {
    for(auto road_index : roads_ind) {
        if(IsValidPositionOnRoad(roads_.at(road_index), position)) {
            return true;
        }
    }
    return false;
};

bool Roadmap::IsValidPositionOnRoad(const Road& road, const geom::Point2D& position) const {
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
    return ((position.x > start_x) || (std::abs(position.x - start_x) < EPSILON)) &&
            ((position.x < end_x) || (std::abs(position.x - end_x) < EPSILON)) &&
            ((position.y > start_y) || (std::abs(position.y - start_y) < EPSILON)) &&
            ((position.y < end_y) || (std::abs(position.y - end_y) < EPSILON));
};

void Roadmap::CopyContent(const Roadmap::Roads& roads) {
    for(auto& road : roads) {
        AddRoad(road);
    }
};

}