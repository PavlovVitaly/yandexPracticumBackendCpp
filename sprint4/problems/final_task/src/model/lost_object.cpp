#include "lost_object.h"

namespace model {

const LostObject::Id& LostObject::GetId() const {
    return id_;
};

void LostObject::SetId(const LostObject::Id& id) {
    id_ = id;
};

size_t LostObject::GetType() const {
    return type_;
};

void LostObject::SetType(size_t type) {
    type_ = type;
};

size_t LostObject::GetValue() const {
    return value_;
};

void LostObject::SetValue(size_t value) {
    value_ = value;
};

}