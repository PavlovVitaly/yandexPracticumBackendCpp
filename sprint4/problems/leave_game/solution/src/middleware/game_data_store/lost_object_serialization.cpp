#include "lost_object_serialization.h"

namespace game_data_ser {

model::LostObject LostObjectSerialization::Restore() const {
    model::LostObject lost_object;
    lost_object.SetId(model::LostObject::Id{id_});
    lost_object.SetType(type_);
    lost_object.SetValue(value_);
    lost_object.SetPosition(position_);
    return lost_object;
};

}