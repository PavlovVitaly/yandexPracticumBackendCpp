#include "json_loader.h"
#include "json_key_storage.h"
#include <fstream>
#include <json/json.h>
#include <iostream>

namespace json_loader {

using namespace Json;

/*
# jsoncpp из conan собран под cxx11, а проект на cxx20, поэтому парсинг и работа со строками напрямую через std::string не работает.
# Решить данную проблему можно либо собрав по jsoncpp под cxx20, сделать из него conan пакет и выложить в репозиторий, либо собирать jsoncpp вместе с основным проектом.
*/
Value ReadFile(const std::filesystem::path& json_path){
    std::ifstream file(json_path);
    if (!file.is_open()){
        std::cout << "Error: Can't open file." << std::endl;    // todo: handler.
        std::exit(1);
    }
    Value root;
    file >> root;
    return root;
};

/* Temp handler, after logger implementation it will need rework. */
void CheckElementExisting(Value& node, const std::string& name){
    if (!node.find(&name[0], &name[name.size()])){
        std::cout << "Error: Element: " << name << "doesn't exist." << std::endl;
        std::exit(1);
    }
}

bool HasElement(Value& node, const std::string& name){
    return node.find(&name[0], &name[name.size()]);
}

void AddRoadsToMap(model::Map& map, Value& map_item){
    CheckElementExisting(map_item, json_keys::ROADS);
    for(auto item : map_item[json_keys::ROADS]){
        CheckElementExisting(item, json_keys::ROAD_XO);
        model::Coord x{item[json_keys::ROAD_XO].asInt()};

        CheckElementExisting(item, json_keys::ROAD_YO);
        model::Coord y{item[json_keys::ROAD_YO].asInt()};

        model::Point start_point(x, y);
        if(HasElement(item, json_keys::ROAD_X1)){
            model::Coord end{item[json_keys::ROAD_X1].asInt()};
            map.AddRoad(model::Road(model::Road::HORIZONTAL, start_point, end));  
        }else{
            CheckElementExisting(item, json_keys::ROAD_Y1);
            model::Coord end{item[json_keys::ROAD_Y1].asInt()};
            map.AddRoad(model::Road(model::Road::VERTICAL, start_point, end));
        }
    }
};

void AddBuildingsToMap(model::Map& map, Value& map_item){
    CheckElementExisting(map_item, json_keys::BUILDINGS);
    for(auto item : map_item[json_keys::BUILDINGS]){
        CheckElementExisting(item, json_keys::BUILDING_X);
        model::Coord x{item[json_keys::BUILDING_X].asInt()};

        CheckElementExisting(item, json_keys::BUILDING_Y);
        model::Coord y{item[json_keys::BUILDING_Y].asInt()};

        CheckElementExisting(item, json_keys::BUILDING_WIDTH);
        model::Coord w{item[json_keys::BUILDING_WIDTH].asInt()};

        CheckElementExisting(item, json_keys::BUILDING_HEIGHT);
        model::Coord h{item[json_keys::BUILDING_HEIGHT].asInt()};

        model::Rectangle rect{model::Point{x, y}, model::Size{w, h}};
        map.AddBuilding(model::Building(std::move(rect)));
    }
};

void AddOfficesToMap(model::Map& map, Value& map_item){
    CheckElementExisting(map_item, json_keys::OFFICES);
    for(auto item : map_item[json_keys::OFFICES]){
        CheckElementExisting(item, json_keys::OFFICE_ID);
        model::Office::Id id_office(item[json_keys::OFFICE_ID].asCString());

        CheckElementExisting(item, json_keys::OFFICE_X);
        model::Coord x{item[json_keys::OFFICE_X].asInt()};

        CheckElementExisting(item, json_keys::OFFICE_Y);
        model::Coord y{item[json_keys::OFFICE_Y].asInt()};

        CheckElementExisting(item, json_keys::OFFICE_OFFSET_X);
        model::Coord dx{item[json_keys::OFFICE_OFFSET_X].asInt()};

        CheckElementExisting(item, json_keys::OFFICE_OFFSET_Y);
        model::Coord dy{item[json_keys::OFFICE_OFFSET_Y].asInt()};

        map.AddOffice(model::Office(id_office,
                                    model::Point(x, y),
                                    model::Offset(dx, dy)));
    }
};

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
    // Распарсить строку как JSON, используя boost::json::parse
    // Загрузить модель игры из файла
    Value map_json = ReadFile(json_path);
    model::Game game;

    CheckElementExisting(map_json, json_keys::MAPS);
    for(auto map_item : map_json[json_keys::MAPS]){
        CheckElementExisting(map_item, json_keys::MAP_ID);
        std::string id(map_item[json_keys::MAP_ID].asCString());

        CheckElementExisting(map_item, json_keys::MAP_NAME);
        model::Map map(model::Map::Id(id), map_item[json_keys::MAP_NAME].asCString());

        AddRoadsToMap(map, map_item);
        AddBuildingsToMap(map, map_item);
        AddOfficesToMap(map, map_item);

        game.AddMap(std::move(map));
    }
    return game;
};

}  // namespace json_loader
