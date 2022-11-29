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
    return roads_;
}

const Map::Offices& Map::GetOffices() const noexcept {
    return offices_;
}

void Map::AddRoad(const Road& road) {
    roads_.emplace_back(road);
}

void Map::AddRoads(Roads& roads){
    for(auto item : roads){
        AddRoad(item);
    }
}

void Map::AddBuilding(const Building& building) {
    buildings_.emplace_back(building);
}

void Map::AddBuildings(Buildings& buildings){
    for(auto item : buildings){
        AddBuilding(item);
    }
}

void Map::AddOffice(Office office) {
    if (warehouse_id_to_index_.contains(office.GetId())) {
        throw std::invalid_argument("Duplicate warehouse");
    }

    const size_t index = offices_.size();
    Office& o = offices_.emplace_back(std::move(office));
    try {
        warehouse_id_to_index_.emplace(o.GetId(), index);
    } catch (...) {
        // Удаляем офис из вектора, если не удалось вставить в unordered_map
        offices_.pop_back();
        throw;
    }
}

void Map::AddOffices(Offices& offices){
    for(auto item : offices){
        AddOffice(item);
    }
}
    
void Map::SetDogVelocity(double velocity) {
    dog_velocity_ = std::abs(velocity);
};

double Map::GetDogVelocity() const noexcept {
    return dog_velocity_;
};

bool Map::IsValidPosition(Position position) {
    return false;
};


void tag_invoke(json::value_from_tag, json::value& jv, const Road& road) {
    if(road.IsHorizontal()) {
        jv = {{ROAD_XO, json::value_from(road.GetStart().x)},
                {ROAD_YO, json::value_from(road.GetStart().y)},
                {ROAD_X1, json::value_from(road.GetEnd().x)}};
    } else {
        jv = {{ROAD_XO, json::value_from(road.GetStart().x)},
                {ROAD_YO, json::value_from(road.GetStart().y)},
                {ROAD_Y1, json::value_from(road.GetEnd().y)}};
    }
};

Road tag_invoke(json::value_to_tag<Road>, const json::value& jv) {
    Point start;
    start.x = json::value_to<int>(jv.as_object().at(ROAD_XO));
    start.y = json::value_to<int>(jv.as_object().at(ROAD_YO));
    Coord end;
    try {
        end = json::value_to<int>(jv.as_object().at(ROAD_X1));
        return Road(Road::HORIZONTAL, start, end);
    } catch (...) {
        end = json::value_to<int>(jv.as_object().at(ROAD_Y1));
        return Road(Road::VERTICAL, start, end);
    }
};


void tag_invoke(json::value_from_tag, json::value& jv, const Building& building) {
    jv = {{BUILDING_X, json::value_from(building.GetBounds().position.x)},
            {BUILDING_Y, json::value_from(building.GetBounds().position.y)},
            {BUILDING_WIDTH, json::value_from(building.GetBounds().size.width)},
            {BUILDING_HEIGHT, json::value_from(building.GetBounds().size.height)}};
};

Building tag_invoke(json::value_to_tag<Building>, const json::value& jv) {
    Point point;
    point.x = json::value_to<int>(jv.as_object().at(BUILDING_X));
    point.y = json::value_to<int>(jv.as_object().at(BUILDING_Y));
    Size size;
    size.width = json::value_to<int>(jv.as_object().at(BUILDING_WIDTH));
    size.height = json::value_to<int>(jv.as_object().at(BUILDING_HEIGHT));
    return Building(Rectangle(point, size));
};


void tag_invoke(json::value_from_tag, json::value& jv, const Office& office) {
    jv = {{OFFICE_ID, json::value_from(*(office.GetId()))},
            {OFFICE_X, json::value_from(office.GetPosition().x)},
            {OFFICE_Y, json::value_from(office.GetPosition().y)},
            {OFFICE_OFFSET_X, json::value_from(office.GetOffset().dx)},
            {OFFICE_OFFSET_Y, json::value_from(office.GetOffset().dy)}};
};

Office tag_invoke(json::value_to_tag<Office>, const json::value& jv) {
    Office::Id id{json::value_to<std::string>(jv.as_object().at(OFFICE_ID))};
    Point position;
    position.x = json::value_to<int>(jv.as_object().at(OFFICE_X));
    position.y = json::value_to<int>(jv.as_object().at(OFFICE_Y));
    Offset offset;
    offset.dx = json::value_to<int>(jv.as_object().at(OFFICE_OFFSET_X));
    offset.dy = json::value_to<int>(jv.as_object().at(OFFICE_OFFSET_Y));
    return Office(id, position, offset);
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
