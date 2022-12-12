#include "map.h"
#include "model_key_storage.h"

#include <stdexcept>

namespace model {
using namespace std::literals;


const Map::Id& Map::GetId() const noexcept {
    return id_;
}

const std::string& Map::GetName() const noexcept {
    return name_;
}

const Map::Buildings& Map::GetBuildings() const noexcept {
    return buildings_;
}

const Map::Roads& Map::GetRoads() const noexcept {
    return roadmap_.GetRoads();
}

const Map::Offices& Map::GetOffices() const noexcept {
    return offices_;
}

void Map::AddRoad(const Road& road) {
    roadmap_.AddRoad(road);
}

void Map::AddRoads(const Roads& roads){
    for(auto item : roads){
        AddRoad(item);
    }
}

void Map::AddBuilding(const Building& building) {
    buildings_.emplace_back(building);
}

void Map::AddBuildings(const Buildings& buildings){
    for(auto item : buildings){
        AddBuilding(item);
    }
}

void Map::AddOffice(const Office& office) {
    if (warehouse_id_to_index_.contains(office.GetId())) {
        throw std::invalid_argument("Duplicate warehouse");
    }

    const size_t index = offices_.size();
    Office& o = offices_.emplace_back(office);
    try {
        warehouse_id_to_index_.emplace(o.GetId(), index);
    } catch (...) {
        // Удаляем офис из вектора, если не удалось вставить в unordered_map
        offices_.pop_back();
        throw;
    }
}

void Map::AddOffices(const Offices& offices){
    for(auto item : offices){
        AddOffice(item);
    }
}
    
void Map::SetDogVelocity(const double velocity) {
    dog_velocity_ = std::abs(velocity);
};

double Map::GetDogVelocity() const noexcept {
    return dog_velocity_;
};

std::tuple<Position, Velocity> Map::GetValidMove(const Position& old_position,
                                                const Position& potential_new_position,
                                                const Velocity& old_velocity) {
    return roadmap_.GetValidMove(old_position, potential_new_position, old_velocity);
};


void tag_invoke(json::value_from_tag, json::value& jv, const Map& map) {
    jv = {{MAP_ID, json::value_from(*(map.GetId()))},
            {MAP_NAME, json::value_from(map.GetName())},
            //{MAP_DOG_VELOCITY, json::value_from(map.GetDogVelocity())}, // todo: need?
            {ROADS, json::value_from(map.GetRoads())},
            {BUILDINGS, json::value_from(map.GetBuildings())},
            {OFFICES, json::value_from(map.GetOffices())}};
};

Map tag_invoke(json::value_to_tag<Map>, const json::value& jv) {
    Map::Id id{json::value_to<std::string>(jv.as_object().at(MAP_ID))};
    std::string name = json::value_to<std::string>(jv.as_object().at(MAP_NAME));
    Map map(id, name);
    std::vector<Road> roads = json::value_to< std::vector<Road> >(jv.as_object().at(ROADS));
    map.AddRoads(roads);
    std::vector<Building> buildings = json::value_to<std::vector<Building>>(jv.as_object().at(BUILDINGS));
    map.AddBuildings(buildings);
    std::vector<Office> offices = json::value_to<std::vector<Office>>(jv.as_object().at(OFFICES));
    map.AddOffices(offices);
    try {
        double dog_velocity = json::value_to<double>(jv.as_object().at(MAP_DOG_VELOCITY));
        map.SetDogVelocity(dog_velocity);
    } catch(...) {}
    return map;
};

}  // namespace model
