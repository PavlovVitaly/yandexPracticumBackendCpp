#pragma once
#include "loot_generator_config.h"
#include "loot_type.h"
#include "building.h"
#include "office.h"
#include "map.h"

#include <boost/json.hpp>

namespace model {

namespace json = boost::json;

LootGeneratorConfig tag_invoke(json::value_to_tag<LootGeneratorConfig>, const json::value& jv);

void tag_invoke(json::value_from_tag, json::value& jv, const LootType& loot_type);
LootType tag_invoke(json::value_to_tag<LootType>, const json::value& jv);

void tag_invoke(json::value_from_tag, json::value& jv, const Building& building);
Building tag_invoke(json::value_to_tag<Building>, const json::value& jv);

void tag_invoke(json::value_from_tag, json::value& jv, const Office& office);
Office tag_invoke(json::value_to_tag<Office>, const json::value& jv);

void tag_invoke(json::value_from_tag, json::value& jv, const Road& road);
Road tag_invoke(json::value_to_tag<Road>, const json::value& jv);

void tag_invoke(json::value_from_tag, json::value& jv, const Map& map);
Map tag_invoke(json::value_to_tag<Map>, const json::value& jv);

}