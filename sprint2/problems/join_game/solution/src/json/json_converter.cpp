#include "json_converter.h"
#include "model_key_storage.h"
#include "json_key_storage.h"
#include <map>
#include <sstream>
#include <boost/json/array.hpp>
#include <boost/json.hpp>

namespace json_converter{

namespace json = boost::json;

std::string ConvertMapListToJson(const model::Game::Maps& maps) {
    json::array mapsArr;
    for(auto map : maps) {
        json::value item = {{model::MAP_ID, *(map->GetId())},
                            {model::MAP_NAME, map->GetName()}};
        mapsArr.push_back(item);
    }
    return json::serialize(mapsArr);
}

std::string ConvertMapToJson(const model::Map& map) {
    return json::serialize(json::value_from(map));
}

std::string CreateMapNotFoundResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "mapNotFound"},
                        {json_keys::RESPONSE_MESSAGE, "Map not found"}};
    return json::serialize(msg);
};

std::string CreateBadRequestResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "badRequest"},
                        {json_keys::RESPONSE_MESSAGE, "Bad request"}};
    return json::serialize(msg);
};

std::string CreatePageNotFoundResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "pageNotFound"},
                        {json_keys::RESPONSE_MESSAGE, "Page not found"}};
    return json::serialize(msg);
};

std::string CreateOnlyPostMethodAllowedResponse(){
    json::value msg = {{json_keys::RESPONSE_CODE, "invalidMethod"},
                        {json_keys::RESPONSE_MESSAGE, "Only POST method is expected"}};
    return json::serialize(msg);
};

std::string CreateJoinToGameInvalidArgumentResponse(){
    json::value msg = {{json_keys::RESPONSE_CODE, "invalidArgument"},
                        {json_keys::RESPONSE_MESSAGE, "Join game request parse error"}};
    return json::serialize(msg);
};

std::string CreateJoinToGameMapNotFoundResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "mapNotFound"},
                        {json_keys::RESPONSE_MESSAGE, "Map not found"}};
    return json::serialize(msg);
};

std::string CreateJoinToGameEmptyPlayerNameResponse(){
    json::value msg = {{json_keys::RESPONSE_CODE, "invalidArgument"},
                        {json_keys::RESPONSE_MESSAGE, "Invalid name"}};
    return json::serialize(msg);
};


std::string CreateJoinToGameResponse(const std::string& token, size_t player_id) {
    json::value msg = {{json_keys::RESPONSE_AUTHORISATION_TOKEN, token},
                        {json_keys::RESPONSE_PLAYER_ID, player_id}};
    return json::serialize(msg);
};

std::optional< std::tuple<std::string, model::Map::Id> > ParseJoinToGameRequest(const std::string& msg) {
    try {
        json::value jv = json::parse(msg);
        std::string player_name = json::value_to<std::string>(jv.as_object().at(json_keys::REQUEST_PLAYER_NAME));
        model::Map::Id map_id{json::value_to<std::string>(jv.as_object().at(json_keys::REQUEST_MAP_ID))};
        return std::tie(player_name, map_id);
    } catch(...) {
        return std::nullopt;
    }
};

}