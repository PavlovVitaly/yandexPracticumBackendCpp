#include "dog_serialization.h"

namespace game_data_ser {

model::Dog DogSerialization::Restore() const {
    model::Dog dog(model::Dog::Id{id_}, name_, bag_capacity_);
    dog.SetDirection(static_cast<model::Direction>(direction_));
    dog.SetPosition(position_);
    std::ranges::for_each(bag_
        , [&dog](const LostObjectSerialization& lost_obj_ser) {
            dog.CollectLostObject(std::make_shared<model::LostObject>(lost_obj_ser.Restore()));
        }
    );
    return dog;
};

}