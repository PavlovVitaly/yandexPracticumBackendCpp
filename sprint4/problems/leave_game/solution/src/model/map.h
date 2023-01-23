#pragma once
#include "tagged.h"
#include "road.h"
#include "building.h"
#include "office.h"
#include "roadmap.h"
#include "loot_type.h"
#include "support_types.h"


#include <tuple>
#include <cmath>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <optional>

namespace model {

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;
    using LootTypes = std::vector<LootType>;

    Map(Id id, std::string name) noexcept
        : id_(std::move(id))
        , name_(std::move(name)) {
    }

    const Id& GetId() const noexcept;
    const std::string& GetName() const noexcept;
    const Buildings& GetBuildings() const noexcept;
    const Roads& GetRoads() const noexcept;
    const Offices& GetOffices() const noexcept;
    const LootTypes& GetLootTypes() const noexcept;
    void AddRoad(const Road& road);
    void AddRoads(const Roads& roads);
    void AddBuilding(const Building& building);
    void AddBuildings(const Buildings& buildings);
    void AddOffice(const Office& office);
    void AddOffices(const Offices& offices);
    void SetDogVelocity(const double velocity);
    double GetDogVelocity() const noexcept;
    void AddLootType(const LootType& loot_type);
    void AddLootTypes(const LootTypes& loot_types);
    std::tuple<geom::Point2D, Velocity> GetValidMove(const geom::Point2D& old_position,
                            const geom::Point2D& potential_new_position,
                            const Velocity& old_velocity) const;
    geom::Point2D GenerateRandomPosition() const;
    size_t GetNumberOfLootTypes() const noexcept;
    const LootType& GetLootTypeBy(size_t id) const;
    void SetBagCapacity(size_t bag_capacity);
    size_t GetBagCapacity() const noexcept;

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roadmap roadmap_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;

    LootTypes loot_types_;

    std::optional<double> dog_velocity_{std::nullopt};
    std::optional<size_t> bag_capacity_{std::nullopt};
};


class EmptyLootTypesOnMapException : public std::exception {
public:
    EmptyLootTypesOnMapException(std::string map_id): map_id_(map_id) {}

    char const* what () {
        std::stringstream ss;
        ss << "No loot type on the map: " << map_id_ << " ."; 
        return  ss.str().c_str();
    }
private:
    std::string map_id_;
};

}  // namespace model
