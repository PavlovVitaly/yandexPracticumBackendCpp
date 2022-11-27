#include "dog.h"

namespace model {

const Dog::Id& Dog::GetId() const {
    return id_;
};

const std::string& Dog::GetName() const {
    return name_;
};

}