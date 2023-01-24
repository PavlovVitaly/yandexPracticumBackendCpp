#include "dog.h"
#include "model_invariants.h"

namespace model {

void Dog::SetMaxInactiveTime(size_t max_inactive_time_in_seconds) {
    max_inactive_time_ = std::chrono::seconds(max_inactive_time_in_seconds);
};

const Dog::Id& Dog::GetId() const {
    return id_;
};

const std::string& Dog::GetName() const {
    return name_;
};

void Dog::SetDirection(Direction direction) {
    direction_ = std::move(direction);
};

const Direction Dog::GetDirection() const {
    return direction_;
};

void Dog::SetPosition(geom::Point2D position) {
    position_ = std::move(position);
    gatherer_.start_pos = gatherer_.end_pos;
    gatherer_.end_pos = position_;
};

const geom::Point2D& Dog::GetPosition() const {
    return position_;
};

void Dog::SetVelocity(Velocity velocity) {
    velocity_ = velocity;
    UpdateDogState(velocity);
};

const Velocity& Dog::GetVelocity() const {
    return velocity_;
};

void Dog::SetAction(Direction direction, double velocity) {
    if((!isInactiveCommandRun && direction == Direction::NONE) 
        || direction != Direction::NONE) {
        inactive_time_ = std::chrono::milliseconds{0};
    }
    isInactiveCommandRun = (direction == Direction::NONE);
    switch(direction){
        case Direction::NORTH: {
            SetDirection(direction);
            SetVelocity({0, -velocity});
            break;
        }
        case Direction::SOUTH: {
            SetDirection(direction);
            SetVelocity({0, velocity});
            break;
        }
        case Direction::WEST: {
            SetDirection(direction);
            SetVelocity({-velocity, 0});
            break;
        }
        case Direction::EAST: {
            SetDirection(direction);
            SetVelocity({velocity, 0});
            break;
        }
        case Direction::NONE: {
            SetVelocity({0, 0});
            break;
        }
    }
};

geom::Point2D Dog::CalculateNewPosition(const std::chrono::milliseconds& delta_time) {
    geom::Point2D position = GetPosition();
    const Velocity& velocity = GetVelocity();
    position.x += velocity.vx * delta_time.count() / MILLISECONDS_IN_SECOND;
    position.y += velocity.vy * delta_time.count() / MILLISECONDS_IN_SECOND;
    return position;
};

void Dog::MakeDogAction(
        const geom::Point2D& new_position,
        const Velocity new_velocity,
        const std::chrono::milliseconds& delta_time) {
    SetPosition(new_position);
    SetVelocity(new_velocity);
    live_time_ += delta_time;
    if(isInactiveCommandRun && state_ == DogState::INACTIVE) {
        inactive_time_ += delta_time;
    }
};

const Dog::BagType& Dog::GetBag() const {
    return bag_;
};

size_t Dog::GetBagCapacity() const {
    return bag_capacity_;
};

void Dog::CollectLostObject(std::shared_ptr<LostObject> loot) {
    if(bag_.size() < bag_capacity_) {
        bag_.push_back(loot);
    }
};

bool Dog::IsFullBag() {
    return bag_.size() >= bag_capacity_;
};

bool Dog::IsEmptyBag() {
    return bag_.empty();
};

void Dog::DropLostObjectsFromBag() {
    for(auto lost_obj : bag_) {
        AddScore(lost_obj->GetValue());
    }
    bag_.clear();
};

std::optional<std::chrono::seconds> Dog::GetPlayTime() {
    if(state_ == DogState::ACTIVE
    || inactive_time_ < max_inactive_time_) {
        return std::nullopt;
    }
    return std::chrono::duration_cast<std::chrono::seconds>(live_time_);
};

const size_t Dog::GetScore() const {
    return score_;
};

void Dog::AddScore(size_t score) {
    score_ += score;
};

const collision_detector::Gatherer& Dog::AsGatherer() const {
    return gatherer_;
};

void Dog::UpdateDogState(const Velocity& new_velocity) {
    if(new_velocity != Velocity{0, 0}) {
        state_ = DogState::ACTIVE;
    } else {
        state_ = DogState::INACTIVE;
    }
};

}