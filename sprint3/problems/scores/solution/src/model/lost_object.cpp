#include "lost_object.h"

namespace model {

const LostObject::Id& LostObject::GetId() const {
    return id_;
};

size_t LostObject::GetType() const {
    return type_;
};

void LostObject::SetType(size_t type) {
    type_ = type;
};

const geom::Point2D& LostObject::GetPosition() const {
    return position_;
};

void LostObject::SetPosition(geom::Point2D position) {
    position_ = std::move(position);
    item_.position = position_;
};

size_t LostObject::GetValue() const {
    return value_;
};

void LostObject::SetValue(size_t value) {
    value_ = value;
};

const collision_detector::Item& LostObject::AsItem() const {
    return item_;
};

}