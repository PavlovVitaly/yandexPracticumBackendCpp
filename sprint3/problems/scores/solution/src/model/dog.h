#pragma once
#include "tagged.h"
#include "support_types.h"
#include "lost_object.h"
#include "collision_detector.h"
#include "geom.h"
#include "model_invariants.h"

#include <string>
#include <unordered_map>
#include <chrono>

namespace model {


class Dog {
    inline static size_t max_id_cont_ = 0;
public:
    using Id = util::Tagged<size_t, Dog>;
    using BagType = std::vector<LostObject>;

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

    void SetDirection(Direction direction);
    const Direction GetDirection() const;

    void SetPosition(geom::Point2D position);
    const geom::Point2D& GetPosition() const;

    void SetVelocity(Velocity velocity);
    const Velocity& GetVelocity() const;
    
    void SetAction(Direction direction, double velocity);
    geom::Point2D CalculateNewPosition(const std::chrono::milliseconds& delta_time);

    const BagType& GetBag() const;

    const size_t GetScore() const;

    const collision_detector::Gatherer& AsGatherer();
private:
    Id id_;
    std::string name_;
    Direction direction_{Direction::NORTH};
    geom::Point2D position_{0.0, 0.0};
    Velocity velocity_{0.0, 0.0};
    BagType bag_;
    collision_detector::Gatherer gatherer_{
        {0.0, 0.0},
        {0.0, 0.0},
        DOG_WIDTH
    };
    size_t score_{0};
};

}