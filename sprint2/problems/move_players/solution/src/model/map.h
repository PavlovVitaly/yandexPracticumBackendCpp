#pragma once
#include "tagged.h"

#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>
#include <boost/json.hpp>

namespace model {

using Dimension = int;
using Coord = Dimension;
namespace json = boost::json;

const double INITIAL_DOG_VELOCITY = 1.0;

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

class Road {
    struct HorizontalTag {
        explicit HorizontalTag() = default;
    };

    struct VerticalTag {
        explicit VerticalTag() = default;
    };

public:
    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}
        , end_{end_x, start.y} {
    }

    Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}
        , end_{start.x, end_y} {
    }

    bool IsHorizontal() const noexcept {
        return start_.y == end_.y;
    }

    bool IsVertical() const noexcept {
        return start_.x == end_.x;
    }

    Point GetStart() const noexcept {
        return start_;
    }

    Point GetEnd() const noexcept {
        return end_;
    }

private:
    Point start_;
    Point end_;
};

void tag_invoke(json::value_from_tag, json::value& jv, const Road& road);
Road tag_invoke(json::value_to_tag<Road>, const json::value& jv);

class Building {
public:
    explicit Building(Rectangle bounds) noexcept
        : bounds_{bounds} {
    }

    const Rectangle& GetBounds() const noexcept {
        return bounds_;
    }

private:
    Rectangle bounds_;
};

void tag_invoke(json::value_from_tag, json::value& jv, const Building& building);
Building tag_invoke(json::value_to_tag<Building>, const json::value& jv);

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}
        , position_{position}
        , offset_{offset} {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    Point GetPosition() const noexcept {
        return position_;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

private:
    Id id_;
    Point position_;
    Offset offset_;
};

void tag_invoke(json::value_from_tag, json::value& jv, const Office& office);
Office tag_invoke(json::value_to_tag<Office>, const json::value& jv);

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;

    Map(Id id, std::string name) noexcept
        : id_(std::move(id))
        , name_(std::move(name)) {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    const Buildings& GetBuildings() const noexcept {
        return buildings_;
    }

    const Roads& GetRoads() const noexcept {
        return roads_;
    }

    const Offices& GetOffices() const noexcept {
        return offices_;
    }

    void AddRoad(const Road& road) {
        roads_.emplace_back(road);
    }

    void AddRoads(Roads& roads){
        for(auto item : roads){
            AddRoad(item);
        }
    }

    void AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void AddBuildings(Buildings& buildings){
        for(auto item : buildings){
            AddBuilding(item);
        }
    }

    void AddOffice(Office office);

    void AddOffices(Offices& offices){
        for(auto item : offices){
            AddOffice(item);
        }
    }
    
    void SetDogVelocity(double velocity) {
        dog_velocity_ = std::abs(velocity);
    };

    double GetDogVelocity() const noexcept {
        return dog_velocity_;
    };

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;

    double dog_velocity_{INITIAL_DOG_VELOCITY};
};

void tag_invoke(json::value_from_tag, json::value& jv, const Map& map);
Map tag_invoke(json::value_to_tag<Map>, const json::value& jv);

}  // namespace model
