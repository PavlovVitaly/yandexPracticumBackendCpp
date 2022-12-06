#include "road.h"
#include "model_key_storage.h"

namespace model {

void tag_invoke(json::value_from_tag, json::value& jv, const Road& road) {
    if(road.IsHorizontal()) {
        jv = {{ROAD_XO, json::value_from(road.GetStart().x)},
                {ROAD_YO, json::value_from(road.GetStart().y)},
                {ROAD_X1, json::value_from(road.GetEnd().x)}};
    } else {
        jv = {{ROAD_XO, json::value_from(road.GetStart().x)},
                {ROAD_YO, json::value_from(road.GetStart().y)},
                {ROAD_Y1, json::value_from(road.GetEnd().y)}};
    }
};

Road tag_invoke(json::value_to_tag<Road>, const json::value& jv) {
    Point start;
    start.x = json::value_to<int>(jv.as_object().at(ROAD_XO));
    start.y = json::value_to<int>(jv.as_object().at(ROAD_YO));
    Coord end;
    try {
        end = json::value_to<int>(jv.as_object().at(ROAD_X1));
        return Road(Road::HORIZONTAL, start, end);
    } catch (...) {
        end = json::value_to<int>(jv.as_object().at(ROAD_Y1));
        return Road(Road::VERTICAL, start, end);
    }
};

}