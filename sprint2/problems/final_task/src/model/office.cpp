#include "office.h"
#include "model_key_storage.h"

namespace model {

void tag_invoke(json::value_from_tag, json::value& jv, const Office& office) {
    jv = {{OFFICE_ID, json::value_from(*(office.GetId()))},
            {OFFICE_X, json::value_from(office.GetPosition().x)},
            {OFFICE_Y, json::value_from(office.GetPosition().y)},
            {OFFICE_OFFSET_X, json::value_from(office.GetOffset().dx)},
            {OFFICE_OFFSET_Y, json::value_from(office.GetOffset().dy)}};
};

Office tag_invoke(json::value_to_tag<Office>, const json::value& jv) {
    Office::Id id{json::value_to<std::string>(jv.as_object().at(OFFICE_ID))};
    Point position;
    position.x = json::value_to<int>(jv.as_object().at(OFFICE_X));
    position.y = json::value_to<int>(jv.as_object().at(OFFICE_Y));
    Offset offset;
    offset.dx = json::value_to<int>(jv.as_object().at(OFFICE_OFFSET_X));
    offset.dy = json::value_to<int>(jv.as_object().at(OFFICE_OFFSET_Y));
    return Office(id, position, offset);
};

}