#pragma once
#include "dog.h"
#include "lost_object_serialization.h"

#include <algorithm>
#include <boost/serialization/vector.hpp>

namespace game_data_ser {

class DogSerialization {
public:
    DogSerialization() = default;
    DogSerialization(const model::Dog& dog):
        id_(*dog.GetId()),
        name_(dog.GetName()),
        direction_(static_cast<int>(dog.GetDirection())),
        position_(dog.GetPosition()),
        score_(dog.GetScore()),
        bag_capacity_(dog.GetBagCapacity()) {
            std::ranges::transform(dog.GetBag(), std::back_inserter(bag_),
                [](std::shared_ptr<model::LostObject> lost_object)->LostObjectSerialization {
                    return *lost_object;
                }
            );
        };

    [[nodiscard]] model::Dog Restore() const;

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar& id_;
        ar& name_;
        ar& direction_;
        ar& position_;
        ar& bag_;
        ar& score_;
        ar& bag_capacity_;
    }
private:
    size_t id_;
    std::string name_;
    int direction_;
    geom::Point2D position_{0.0, 0.0};
    size_t score_{0};
    size_t bag_capacity_{0};
    std::vector<LostObjectSerialization> bag_;
};

}