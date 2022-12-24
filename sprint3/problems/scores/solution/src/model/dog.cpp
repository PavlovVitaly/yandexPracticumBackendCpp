#include "dog.h"
#include "model_invariants.h"

namespace model {

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
};

const Velocity& Dog::GetVelocity() const {
    return velocity_;
};

void Dog::SetAction(Direction direction, double velocity) {
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

const Dog::BagType& Dog::GetBag() const {
    return bag_;
};

void Dog::CollectLostObject(std::shared_ptr<LostObject> loot) {
    if(bag_.size() < bag_capacity_) {
        bag_.push_back(loot);
    }
};

bool Dog::IsFullBag() {
    return bag_.size() >= bag_capacity_;
};

const size_t Dog::GetScore() const {
    return score_;
};

void Dog::AddScore(size_t score) {
    score_ += score;
};

void Dog::ClearScore() {
    score_ = 0;
};

const collision_detector::Gatherer& Dog::AsGatherer() const {
    return gatherer_;
};

}