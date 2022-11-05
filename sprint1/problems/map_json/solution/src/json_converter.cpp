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


std::string ConvertMapToJson(const model::Map& map){
    ptree jTree;
    ptree jArray;
    //for(auto item : game.GetMaps()){
    //    ptree map;
    //    map.put(*item.GetId(), item.GetName());
    //    jArray.push_back(std::make_pair("", std::move(map)));
    //}
    //jTree.add_child("dd", jArray);
    std::stringstream jsonString;
    write_json(jsonString, jTree);
    return jsonString.str();
}

}