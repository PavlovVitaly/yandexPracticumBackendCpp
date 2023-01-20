#pragma once
#include "lost_object.h"
#include "misc_serialization.h"

#include <boost/serialization/vector.hpp>

namespace game_data_ser {

class LostObjectSerialization {
public:
    LostObjectSerialization() = default;
    LostObjectSerialization(const model::LostObject& lost_object):
        id_(*lost_object.GetId()),
        type_(lost_object.GetType()),
        value_(lost_object.GetValue()),
        position_(lost_object.GetPosition()) {};

    [[nodiscard]] model::LostObject Restore() const;

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar& id_;
        ar& type_;
        ar& value_;
        ar& position_;
    }
private:
    size_t id_;
    size_t type_;
    size_t value_;
    geom::Point2D position_;
};

}