#include "json_converter.h"
#include "model_key_storage.h"
#include "json_key_storage.h"
#include "json_model_converter.h"

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

std::string CreateOnlyPostMethodAllowedResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "invalidMethod"},
                        {json_keys::RESPONSE_MESSAGE, "Only POST method is expected"}};
    return json::serialize(msg);
};

std::string CreateJoinToGameInvalidArgumentResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "invalidArgument"},
                        {json_keys::RESPONSE_MESSAGE, "Join game request parse error"}};
    return json::serialize(msg);
};

std::string CreateJoinToGameMapNotFoundResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "mapNotFound"},
                        {json_keys::RESPONSE_MESSAGE, "Map not found"}};
    return json::serialize(msg);
};

std::string CreateJoinToGameEmptyPlayerNameResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "invalidArgument"},
                        {json_keys::RESPONSE_MESSAGE, "Invalid name"}};
    return json::serialize(msg);
};

std::string CreateInvalidMethodResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "invalidMethod"},
                        {json_keys::RESPONSE_MESSAGE, "Invalid method"}};
    return json::serialize(msg);
};

std::string CreateEmptyAuthorizationResponse() {
    //json::value msg = {{json_keys::RESPONSE_CODE, "invalidToken"},
    //                    {json_keys::RESPONSE_MESSAGE, "Authorization header is missing"}}; // todo: different messages
    json::value msg = {{json_keys::RESPONSE_CODE, "invalidToken"},
                        {json_keys::RESPONSE_MESSAGE, "Authorization header is required"}};
    return json::serialize(msg);
};

std::string CreateUnknownTokenResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "unknownToken"},
                        {json_keys::RESPONSE_MESSAGE, "Player token has not been found"}};
    return json::serialize(msg);
};

std::string CreatePlayerActionResponse() {
    json::object msg = {};
    return json::serialize(msg);
};

std::string CreatePlayerActionInvalidActionResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "invalidArgument"},
                        {json_keys::RESPONSE_MESSAGE, "Failed to parse action"}};
    return json::serialize(msg);
};

std::string CreateInvalidContentTypeResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "invalidArgument"},
                        {json_keys::RESPONSE_MESSAGE, "Invalid content type"}};
    return json::serialize(msg);
};

std::string CreatePlayersListOnMapResponse(const std::vector< std::shared_ptr<app::Player> >& players) {
    json::value jv;
    json::object& obj = jv.emplace_object();  
    for(auto player : players) {
        std::stringstream ss;
        ss << *(player->GetId());
        json::value jv_item = {{json_keys::RESPONSE_PLAYER_NAME, player->GetName()}};
        obj[ss.str()] = jv_item;
    }
    return json::serialize(jv);
};

std::string CreateGameStateResponse(const std::vector< std::shared_ptr<app::Player> >& players,
                                    const app::GameSession::LostObjects& lost_objects) {
    json::value jv;
    json::object palyers_obj;
    json::object loots; 
    json::object res;
    for(auto player : players) {
        auto dog = player->GetDog().lock();
        std::stringstream ss;
        ss << *(player->GetId());
        json::array pos = {dog->GetPosition().x, dog->GetPosition().y};
        json::array speed = {dog->GetVelocity().vx, dog->GetVelocity().vy};
        json::array bag;
        for(auto item : dog->GetBag()) {
            bag.push_back({{json_keys::RESPONSE_BAG_ITEM_ID, *(item->GetId())},
                            {json_keys::RESPONSE_BAG_ITEM_TYPE, item->GetType()}});
        }
        json::value jv_item = {{json_keys::RESPONSE_DOG_POSITION, pos},
                                {json_keys::RESPONSE_DOG_VELOCITY, speed},
                                {json_keys::RESPONSE_DOG_DIRECTION, model::DIRECTION_TO_STRING.at(dog->GetDirection())},
                                {json_keys::RESPONSE_DOG_BAG, bag},
                                {json_keys::RESPONSE_PLAYER_SCORE, dog->GetScore()}};
        palyers_obj[ss.str()] = jv_item;
    }
    res[json_keys::RESPONSE_PLAYERS] = palyers_obj;

    for(auto [id, lost_object] : lost_objects) {
        std::stringstream ss;
        ss << *id;
        json::value loot = {
            {json_keys::LOST_OBJECTS_TYPE, json::value_from(lost_object->GetType())},
            {json_keys::LOST_OBJECTS_POSITION, json::array{lost_object->GetPosition().x, lost_object->GetPosition().y}}
        };
        loots[ss.str()] = loot;
    }
    res[json_keys::LOST_OBJECTS] = loots;
    jv.emplace_object() = res;
    return json::serialize(jv);
};

std::string CreateJoinToGameResponse(const std::string& token, size_t player_id) {
    json::value msg = {{json_keys::RESPONSE_AUTHORISATION_TOKEN, token},
                        {json_keys::RESPONSE_PLAYER_ID, player_id}};
    return json::serialize(msg);
};

std::string CreateSetDeltaTimeResponse() {
    json::object msg = {};
    return json::serialize(msg);
};

std::string CreateSetDeltaTimeInvalidMsgResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "invalidArgument"},
                        {json_keys::RESPONSE_MESSAGE, "Failed to parse tick request JSON"}};
    return json::serialize(msg);
};

std::string CreateInvalidEndpointResponse() {
    json::value msg = {{json_keys::RESPONSE_CODE, "badRequest"},
                        {json_keys::RESPONSE_MESSAGE, "Invalid endpoint"}};
    return json::serialize(msg);
};

std::string CreateRecordsTableResponse(const std::vector<domain::PlayerRecord>& records_table) {
    json::array player_records_arr;
    for(const auto& player_record : records_table) {
        json::value item = {
            {json_keys::RESPONSE_PLAYER_RECORD_NAME, player_record.GetName()},
            {json_keys::RESPONSE_PLAYER_RECORD_SCORE, player_record.GetScore()},
            {json_keys::RESPONSE_PLAYER_RECORD_PLAY_TIME, player_record.GetPlayTime()}
        };
        player_records_arr.push_back(item);
    }
    return json::serialize(player_records_arr);
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

std::optional<std::string> ParsePlayerActionRequest(const std::string& msg) {
    try {
        json::value jv = json::parse(msg);
        std::string direction = json::value_to<std::string>(jv.as_object().at(json_keys::REQUEST_PLAYER_MOVE));
        return direction;
    } catch(...) {
        return std::nullopt;
    }
};

std::optional<int> ParseSetDeltaTimeRequest(const std::string& msg) {
    try {
        json::value jv = json::parse(msg);
        if(!jv.as_object().at(json_keys::REQUEST_TIME_DELTA).is_int64()) {
            return std::nullopt;
        }
        int time_delta = json::value_to<int>(jv.as_object().at(json_keys::REQUEST_TIME_DELTA));
        return time_delta;
    } catch(...) {
        return std::nullopt;
    }
};


}