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

const Position& LostObject::GetPosition() const {
    return position_;
};

void LostObject::SetPosition(Position position) {
    position_ = std::move(position);
};

}