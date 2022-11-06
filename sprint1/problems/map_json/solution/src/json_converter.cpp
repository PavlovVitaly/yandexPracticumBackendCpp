#include "json_converter.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <map>
#include <sstream>
#include <boost/json.hpp>

namespace jsonConverter{

using boost::property_tree::ptree;
using boost::property_tree::write_json;
namespace json = boost::json;

std::string ConvertMapListToJson(const model::Game& game){
    std::stringstream result;
    result << "[";
    for(auto item = game.GetMaps().begin(); item != game.GetMaps().end(); ++item){
        result << "{\"id\": \"" << *item->GetId() << "\", \"name\": \"" << item->GetName() << "\"}";
        if(std::next(item) != game.GetMaps().end()){
            result << ", ";
        }
    }
    result << "]";
    return result.str();
}

void AddRoadsToJson(const model::Map& map, ptree& root){
    ptree roads;
    for(auto item : map.GetRoads()){
        ptree road;
        road.add("x0", item.GetStart().x);
        road.add("y0", item.GetStart().y);
        if(item.IsVertical()) road.add("y1", item.GetEnd().y);
        else road.add("x1", item.GetEnd().x);
        roads.push_back(std::make_pair("", std::move(road)));
    }
    root.add_child("roads", roads);
}

void AddBuildingsToJson(const model::Map& map, ptree& root){
    ptree buildings;
    for(auto item : map.GetBuildings()){
        ptree building;
        building.add("x", item.GetBounds().position.x);
        building.add("y", item.GetBounds().position.y);
        building.add("w", item.GetBounds().size.width);
        building.add("h", item.GetBounds().size.height);
        buildings.push_back(std::make_pair("", std::move(building)));
    }
    root.add_child("buildings", buildings);
}

void AddOfficesToJson(const model::Map& map, ptree& root){
    ptree offices;
    for(auto item : map.GetOffices()){
        ptree office;
        office.add("id", *item.GetId());
        office.add("x", item.GetPosition().x);
        office.add("y", item.GetPosition().y);
        office.add("offsetX", item.GetOffset().dx);
        office.add("offsetY", item.GetOffset().dy);
        offices.push_back(std::make_pair("", std::move(office)));
    }
    root.add_child("offices", offices);
}

std::string ConvertMapToJson(const model::Map& map){
    ptree root, roads, buildings, offices;
    root.add("id", *map.GetId());
    root.add("name", map.GetName());
    AddRoadsToJson(map, root);
    AddBuildingsToJson(map, root);
    AddOfficesToJson(map, root);
    
    std::stringstream jsonString;
    write_json(jsonString, root);
    return jsonString.str();
}

}