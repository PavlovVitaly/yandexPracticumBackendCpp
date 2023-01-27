#include "json_loader.h"
#include "logger.h"
#include "json_key_storage.h"
#include "model_key_storage.h"
#include "loot_generator_config.h"
#include "json_model_converter.h"

#include <fstream>
#include <string_view>
#include <sstream>

namespace json_loader {

using namespace std::literals;


boost::json::value ReadFile(const std::filesystem::path& json_path) {
    std::ifstream file(json_path);
    if (!file.is_open()) {
        BOOST_LOG_TRIVIAL(error) << logware::CreateLogMessage("error"sv,
                                        logware::ExceptionLogData(EXIT_FAILURE,
                                            "Error: Can't open file."sv,
                                            "write something here"sv)); // todo: write message and handler.
        throw OpenConfigFileOfModelException();
    }
    
    std::stringstream ss;
    ss << file.rdbuf();
    boost::json::value root = boost::json::parse(ss.str());
    return root;
};

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
    // Распарсить строку как JSON, используя boost::json::parse
    // Загрузить модель игры из файла
    model::Game game;
    boost::json::value jsonVal = ReadFile(json_path);
    model::LootGeneratorConfig lootGenCfg = 
        boost::json::value_to<model::LootGeneratorConfig>(jsonVal.as_object().at(model::LOOT_GENERATOR_CONFIG));
    game.AddLootGeneratorConfig(lootGenCfg);
    std::vector<model::Map> maps = boost::json::value_to< std::vector<model::Map> >(jsonVal.as_object().at(model::MAPS));
    game.AddMaps(maps);
    try {
        double default_dog_velocity = boost::json::value_to<double>(jsonVal.as_object().at(model::DEFAULT_DOG_VELOCITY));
        game.SetDefaultDogVelocity(default_dog_velocity);
    } catch(boost::wrapexcept<std::out_of_range>& e) {}
    try {
        double default_bag_capacity = boost::json::value_to<double>(jsonVal.as_object().at(model::DEFAULT_BAG_CAPACITY));
        game.SetDefaultBagCapacity(default_bag_capacity);
    } catch(boost::wrapexcept<std::out_of_range>& e) {}
    try {
        size_t dog_retirement_time = boost::json::value_to<size_t>(jsonVal.as_object().at(model::DOG_RETIREMENT_TIME));
        model::Dog::SetMaxInactiveTime(dog_retirement_time);
    } catch(boost::wrapexcept<std::out_of_range>& e) {}
    return game;
};

}  // namespace json_loader
