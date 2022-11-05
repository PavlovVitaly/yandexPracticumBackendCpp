#include "json_loader.h"
#include <fstream>
#include <boost/json.hpp>

#include <iostream>

namespace json_loader {

namespace json = boost::json;

std::string ReadFile(const std::filesystem::path& json_path){
    std::ifstream file(json_path);
    if (!file.is_open()){
        std::cout << "Error: Can't open file." << std::endl;    // todo: handler.
        std::exit(1);
    }

    std::string tp;
    std::string text;
    while(getline(file, tp)){
        text += tp;
    }
    file.close();
    return text;
};

void AddRoadsToMap(model::Map& map, const json::value& mapItem){
    if(!mapItem.as_object().contains("roads")) return;
    for(auto item : mapItem.as_object().at("roads").as_array()){
        model::Coord x{static_cast<int>(item.as_object().at("x0").as_int64())};
        model::Coord y{static_cast<int>(item.as_object().at("y0").as_int64())};
        model::Point startPoint(x, y);
        if(item.as_object().contains("x1")){
            model::Coord end{static_cast<int>(item.as_object().at("x1").as_int64())};
            map.AddRoad(model::Road(model::Road::HORIZONTAL, startPoint, end));  
        }else{
            model::Coord end{static_cast<int>(item.as_object().at("y1").as_int64())};
            map.AddRoad(model::Road(model::Road::VERTICAL, startPoint, end));
        }
    }
};

void AddBuildingsToMap(model::Map& map, const json::value& mapItem){
    if(!mapItem.as_object().contains("buildings")) return;
    for(auto item : mapItem.as_object().at("buildings").as_array()){
        model::Coord x{static_cast<int>(item.as_object().at("x").as_int64())};
        model::Coord y{static_cast<int>(item.as_object().at("y").as_int64())};
        model::Coord w{static_cast<int>(item.as_object().at("w").as_int64())};
        model::Coord h{static_cast<int>(item.as_object().at("h").as_int64())};
        model::Rectangle rect{model::Point{x, y}, model::Size{w, h}};
        map.AddBuilding(model::Building(std::move(rect)));
    }
};

void AddOfficesToMap(model::Map& map, const json::value& mapItem){
    if(!mapItem.as_object().contains("offices")) return;
    for(auto item : mapItem.as_object().at("offices").as_array()){
        model::Office::Id idOffice(std::string(mapItem.as_object().at("id").as_string()));
        model::Coord x{static_cast<int>(item.as_object().at("x").as_int64())};
        model::Coord y{static_cast<int>(item.as_object().at("y").as_int64())};
        model::Coord dx{static_cast<int>(item.as_object().at("offsetX").as_int64())};
        model::Coord dy{static_cast<int>(item.as_object().at("offsetY").as_int64())};
        map.AddOffice(model::Office(idOffice,
                                    model::Point(x, y),
                                    model::Offset(dx, dy)));
    }
};

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
    // Распарсить строку как JSON, используя boost::json::parse
    // Загрузить модель игры из файла
    std::string text{ReadFile(json_path)};
    auto mapJson = json::parse(text);
    model::Game game;

    for(auto mapItem : mapJson.as_object().at("maps").as_array()){
        std::string id(mapItem.as_object().at("id").as_string());
        std::string name(mapItem.as_object().at("name").as_string());
        model::Map map(model::Map::Id(id), name);
        AddRoadsToMap(map, mapItem);
        AddBuildingsToMap(map, mapItem);
        AddOfficesToMap(map, mapItem);
        game.AddMap(std::move(map));
    }
    return game;
};

}  // namespace json_loader
