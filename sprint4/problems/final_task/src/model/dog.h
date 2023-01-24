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
#include <memory>
#include <optional>

namespace model {


class Dog {
    enum class DogState {ACTIVE, INACTIVE};
    inline static size_t max_id_cont_{0};
    inline static std::chrono::seconds max_inactive_time_{ONE_MINUTE_IN_SECONDS};
public:
    using Id = util::Tagged<size_t, Dog>;
    using BagType = std::vector< std::shared_ptr<LostObject> >;

    static void SetMaxInactiveTime(size_t max_inactive_time_in_seconds);

    Dog() = default;
    Dog(std::string name, size_t bag_capacity) : 
        id_(Id{Dog::max_id_cont_++}),
        name_(name),
        bag_capacity_(bag_capacity) {};
    Dog(Id id, std::string name, size_t bag_capacity) :
        id_(id),
        name_(name),
        bag_capacity_(bag_capacity) {
            if(*id_ >= Dog::max_id_cont_){
                Dog::max_id_cont_ = *id_ + 1;
            }
        };
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

    const Velocity& GetVelocity() const;
    
    void SetAction(Direction direction, double velocity);
    geom::Point2D CalculateNewPosition(const std::chrono::milliseconds& delta_time);
    void MakeDogAction(
        const geom::Point2D& new_position,
        const Velocity new_velocity,
        const std::chrono::milliseconds& delta_time
    );

    const BagType& GetBag() const;
    size_t GetBagCapacity() const;
    void CollectLostObject(std::shared_ptr<LostObject> loot);
    bool IsFullBag();
    bool IsEmptyBag();
    void DropLostObjectsFromBag();
    std::optional<std::chrono::seconds> GetPlayTime();

    const size_t GetScore() const;

    const collision_detector::Gatherer& AsGatherer() const;
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
    size_t bag_capacity_{0};
    std::chrono::milliseconds inactive_time_{0};
    std::chrono::milliseconds live_time_{0};
    bool isInactiveCommandRun{true};
    DogState state_;

    void SetVelocity(Velocity velocity);
    void AddScore(size_t score);
    void UpdateDogState(const Velocity& new_velocity);
};

}