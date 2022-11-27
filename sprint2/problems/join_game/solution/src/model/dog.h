#pragma once
#include "tagged.h"

#include <string>

namespace model {

class Dog {
    inline static size_t max_id_cont_ = 0;
public:
    using Id = util::Tagged<size_t, Dog>;
    Dog(std::string name) : 
        id_(Id{Dog::max_id_cont_++}),
        name_(name) {};
    Dog(Id id, std::string name) :
        id_(id),
        name_(name) {};
    Dog(const Dog& other) = default;
    Dog(Dog&& other) = default;
    Dog& operator = (const Dog& other) = default;
    Dog& operator = (Dog&& other) = default;
    virtual ~Dog() = default;

    const Id& GetId() const;
    const std::string& GetName() const;
private:
    Id id_;
    std::string name_;
};

}