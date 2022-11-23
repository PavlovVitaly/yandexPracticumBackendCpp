#include "json_converter.h"
#include "json_key_storage.h"
#include <map>
#include <sstream>
#include <json/json.h>

namespace json_converter{

std::string ConvertMapListToJson(const model::Game& game) {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    for(auto item : game.GetMaps()) {
        Json::Value map;
        map[json_keys::MAP_ID] = (*item.GetId()).c_str();
        map[json_keys::MAP_NAME] = item.GetName().c_str();
        root.append(std::move(map));
    }
    std::stringstream json_string;
    writer->write(root, &json_string);
    return json_string.str();
}

void AddRoadsToJson(const model::Map& map, Json::Value& root) {
    Json::Value roads;
    for(auto item : map.GetRoads()) {
        Json::Value road;
        road[json_keys::ROAD_XO] = item.GetStart().x;
        road[json_keys::ROAD_YO] = item.GetStart().y;
        if(item.IsVertical()) {
            road[json_keys::ROAD_Y1] = item.GetEnd().y;
        } else {
            road[json_keys::ROAD_X1] = item.GetEnd().x;
        }
        roads.append(std::move(road));
    }
    root[json_keys::ROADS] = roads;
}

void AddBuildingsToJson(const model::Map& map, Json::Value& root) {
    Json::Value buildings;
    for(auto item : map.GetBuildings()) {
        Json::Value building;
        building[json_keys::BUILDING_X] = item.GetBounds().position.x;
        building[json_keys::BUILDING_Y] = item.GetBounds().position.y;
        building[json_keys::BUILDING_WIDTH] = item.GetBounds().size.width;
        building[json_keys::BUILDING_HEIGHT] = item.GetBounds().size.height;
        buildings.append(std::move(building));
    }
    root[json_keys::BUILDINGS] = buildings;
}

void AddOfficesToJson(const model::Map& map, Json::Value& root) {
    Json::Value offices;
    for(auto item : map.GetOffices()) {
        Json::Value office;
        office[json_keys::OFFICE_ID] = (*item.GetId()).c_str();
        office[json_keys::OFFICE_X] = item.GetPosition().x;
        office[json_keys::OFFICE_Y] = item.GetPosition().y;
        office[json_keys::OFFICE_OFFSET_X] = item.GetOffset().dx;
        office[json_keys::OFFICE_OFFSET_Y] = item.GetOffset().dy;
        offices.append(std::move(office));
    }
    root[json_keys::OFFICES] = offices;
}

std::string ConvertMapToJson(const model::Map& map) {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    root[json_keys::MAP_ID] = (*map.GetId()).c_str();
    root[json_keys::MAP_NAME] = map.GetName().c_str();
    AddRoadsToJson(map, root);
    AddBuildingsToJson(map, root);
    AddOfficesToJson(map, root);
    std::stringstream json_string;
    writer->write(root, &json_string);
    return json_string.str();
}

std::string CreateMapNotFoundResponse() {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    root[json_keys::RESPONSE_CODE] = "mapNotFound";
    root[json_keys::RESPONSE_MESSAGE] = "Map not found";
    std::stringstream json_string;
    writer->write(root, &json_string);
    return json_string.str();
};

std::string CreateBadRequestResponse() {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    root[json_keys::RESPONSE_CODE] = "badRequest";
    root[json_keys::RESPONSE_MESSAGE] = "Bad request";
    std::stringstream json_string;
    writer->write(root, &json_string);
    return json_string.str();
};

std::string CreatePageNotFoundResponse() {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    root[json_keys::RESPONSE_CODE] = "pageNotFound";
    root[json_keys::RESPONSE_MESSAGE] = "Page not found";
    std::stringstream json_string;
    writer->write(root, &json_string);
    return json_string.str();
};

}