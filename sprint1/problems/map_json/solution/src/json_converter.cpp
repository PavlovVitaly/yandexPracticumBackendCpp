#include "json_converter.h"
#include <map>
#include <sstream>
#include <json/json.h>

namespace jsonConverter{

std::string ConvertMapListToJson(const model::Game& game){
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    for(auto item : game.GetMaps()){
        Json::Value map;
        map["id"] = (*item.GetId()).c_str();
        map["name"] = item.GetName().c_str();
        root.append(std::move(map));
    }
    std::stringstream jsonString;
    writer->write(root, &jsonString);
    return jsonString.str();
}

void AddRoadsToJson(const model::Map& map, Json::Value& root){
    Json::Value roads;
    for(auto item : map.GetRoads()){
        Json::Value road;
        road["x0"] = item.GetStart().x;
        road["y0"] = item.GetStart().y;
        if(item.IsVertical()) road["y1"] = item.GetEnd().y;
        else road["x1"] = item.GetEnd().x;
        roads.append(std::move(road));
    }
    root["roads"] = roads;
}

void AddBuildingsToJson(const model::Map& map, Json::Value& root){
    Json::Value buildings;
    for(auto item : map.GetBuildings()){
        Json::Value building;
        building["x"] = item.GetBounds().position.x;
        building["y"] = item.GetBounds().position.y;
        building["w"] = item.GetBounds().size.width;
        building["h"] = item.GetBounds().size.height;
        buildings.append(std::move(building));
    }
    root["buildings"] = buildings;
}

void AddOfficesToJson(const model::Map& map, Json::Value& root){
    Json::Value offices;
    for(auto item : map.GetOffices()){
        Json::Value office;
        office["id"] = (*item.GetId()).c_str();
        office["x"] = item.GetPosition().x;
        office["y"] = item.GetPosition().y;
        office["offsetX"] = item.GetOffset().dx;
        office["offsetY"] = item.GetOffset().dy;
        offices.append(std::move(office));
    }
    root["offices"] = offices;
}

std::string ConvertMapToJson(const model::Map& map){
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    root["id"] = (*map.GetId()).c_str();
    root["name"] = map.GetName().c_str();
    AddRoadsToJson(map, root);
    AddBuildingsToJson(map, root);
    AddOfficesToJson(map, root);
    std::stringstream jsonString;
    writer->write(root, &jsonString);
    return jsonString.str();
}

std::string CreateMapNotFoundResponse(){
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    root["code"] = "mapNotFound";
    root["message"] = "Map not found";
    std::stringstream jsonString;
    writer->write(root, &jsonString);
    return jsonString.str();
};

std::string CreateBadRequestResponse(){
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    root["code"] = "badRequest";
    root["message"] = "Bad request";
    std::stringstream jsonString;
    writer->write(root, &jsonString);
    return jsonString.str();
};

std::string CreatePageNotFoundResponse(){
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    root["code"] = "pageNotFound";
    root["message"] = "Page not found";
    std::stringstream jsonString;
    writer->write(root, &jsonString);
    return jsonString.str();
};

}