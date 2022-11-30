#pragma once
#include "tagged.h"
#include "road.h"
#include "building.h"
#include "office.h"
#include "roadmap.h"
#include "support_types.h"


#include <cmath>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <boost/json.hpp>

namespace model {

namespace json = boost::json;

const double INITIAL_DOG_VELOCITY = 1.0;

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

    const Id& GetId() const noexcept;
    const std::string& GetName() const noexcept;
    const Buildings& GetBuildings() const noexcept;
    const Roads& GetRoads() const noexcept;
    const Offices& GetOffices() const noexcept;
    void AddRoad(const Road& road);
    void AddRoads(Roads& roads);
    void AddBuilding(const Building& building);
    void AddBuildings(Buildings& buildings);
    void AddOffice(Office office);
    void AddOffices(Offices& offices);
    void SetDogVelocity(double velocity);
    double GetDogVelocity() const noexcept;
    bool IsValidPosition(const Position& position);

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;

    double dog_velocity_{INITIAL_DOG_VELOCITY};

    std::shared_ptr<Roadmap> roadmap_;
};

void tag_invoke(json::value_from_tag, json::value& jv, const Map& map);
Map tag_invoke(json::value_to_tag<Map>, const json::value& jv);

}  // namespace model
