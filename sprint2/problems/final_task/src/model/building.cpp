#include "building.h"
#include "model_key_storage.h"

namespace model {

void tag_invoke(json::value_from_tag, json::value& jv, const Building& building) {
    jv = {{BUILDING_X, json::value_from(building.GetBounds().position.x)},
            {BUILDING_Y, json::value_from(building.GetBounds().position.y)},
            {BUILDING_WIDTH, json::value_from(building.GetBounds().size.width)},
            {BUILDING_HEIGHT, json::value_from(building.GetBounds().size.height)}};
};

Building tag_invoke(json::value_to_tag<Building>, const json::value& jv) {
    Point point;
    point.x = json::value_to<int>(jv.as_object().at(BUILDING_X));
    point.y = json::value_to<int>(jv.as_object().at(BUILDING_Y));
    Size size;
    size.width = json::value_to<int>(jv.as_object().at(BUILDING_WIDTH));
    size.height = json::value_to<int>(jv.as_object().at(BUILDING_HEIGHT));
    return Building(Rectangle(point, size));
};

}