#include "json_loader.h"
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

void AddRoadsToMap(model::Map& map, Value& mapItem){
    CheckElementExisting(mapItem, "roads");
    for(auto item : mapItem["roads"]){
        CheckElementExisting(item, "x0");
        model::Coord x{item["x0"].asInt()};

        CheckElementExisting(item, "y0");
        model::Coord y{item["y0"].asInt()};

        model::Point startPoint(x, y);
        if(HasElement(item, "x1")){
            model::Coord end{item["x1"].asInt()};
            map.AddRoad(model::Road(model::Road::HORIZONTAL, startPoint, end));  
        }else{
            CheckElementExisting(item, "y1");
            model::Coord end{item["y1"].asInt()};
            map.AddRoad(model::Road(model::Road::VERTICAL, startPoint, end));
        }
    }
};

void AddBuildingsToMap(model::Map& map, Value& mapItem){
    CheckElementExisting(mapItem, "buildings");
    for(auto item : mapItem["buildings"]){
        CheckElementExisting(item, "x");
        model::Coord x{item["x"].asInt()};

        CheckElementExisting(item, "y");
        model::Coord y{item["y"].asInt()};

        CheckElementExisting(item, "w");
        model::Coord w{item["w"].asInt()};

        CheckElementExisting(item, "h");
        model::Coord h{item["h"].asInt()};

        model::Rectangle rect{model::Point{x, y}, model::Size{w, h}};
        map.AddBuilding(model::Building(std::move(rect)));
    }
};

void AddOfficesToMap(model::Map& map, Value& mapItem){
    CheckElementExisting(mapItem, "offices");
    for(auto item : mapItem["offices"]){
        CheckElementExisting(item, "id");
        model::Office::Id idOffice(item["id"].asCString());

        CheckElementExisting(item, "x");
        model::Coord x{item["x"].asInt()};

        CheckElementExisting(item, "y");
        model::Coord y{item["y"].asInt()};

        CheckElementExisting(item, "offsetX");
        model::Coord dx{item["offsetX"].asInt()};

        CheckElementExisting(item, "offsetY");
        model::Coord dy{item["offsetY"].asInt()};

        map.AddOffice(model::Office(idOffice,
                                    model::Point(x, y),
                                    model::Offset(dx, dy)));
    }
};

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
    // Распарсить строку как JSON, используя boost::json::parse
    // Загрузить модель игры из файла
    Value mapJson = ReadFile(json_path);
    model::Game game;

    CheckElementExisting(mapJson, "maps");
    for(auto mapItem : mapJson["maps"]){
        CheckElementExisting(mapItem, "id");
        std::string id(mapItem["id"].asCString());

        CheckElementExisting(mapItem, "name");
        model::Map map(model::Map::Id(id), mapItem["name"].asCString());

        AddRoadsToMap(map, mapItem);
        AddBuildingsToMap(map, mapItem);
        AddOfficesToMap(map, mapItem);

        game.AddMap(std::move(map));
    }
    return game;
};

}  // namespace json_loader
