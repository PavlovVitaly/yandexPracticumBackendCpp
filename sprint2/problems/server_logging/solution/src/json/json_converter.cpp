#include "json_converter.h"
#include "model_key_storage.h"
#include "json_key_storage.h"
#include <map>
#include <sstream>
#include <boost/json/array.hpp>
#include <boost/json.hpp>

namespace json_converter{

std::string ConvertMapListToJson(const model::Game& game) {
    boost::json::array mapsArr;
    for(auto map : game.GetMaps()) {
        boost::json::value item = {{model::MAP_ID, *(map.GetId())},
                                    {model::MAP_NAME, map.GetName()}};
        mapsArr.push_back(item);
    }
    return boost::json::serialize(mapsArr);
}

std::string ConvertMapToJson(const model::Map& map) {
    return boost::json::serialize(boost::json::value_from(map));
}

std::string CreateMapNotFoundResponse() {
    boost::json::value msg = {{json_keys::RESPONSE_CODE, "mapNotFound"},
                                {json_keys::RESPONSE_MESSAGE, "Map not found"}};
    return boost::json::serialize(msg);
};

std::string CreateBadRequestResponse() {
    boost::json::value msg = {{json_keys::RESPONSE_CODE, "badRequest"},
                                {json_keys::RESPONSE_MESSAGE, "Bad request"}};
    return boost::json::serialize(msg);
};

std::string CreatePageNotFoundResponse() {
    boost::json::value msg = {{json_keys::RESPONSE_CODE, "pageNotFound"},
                                {json_keys::RESPONSE_MESSAGE, "Page not found"}};
    return boost::json::serialize(msg);
};

}