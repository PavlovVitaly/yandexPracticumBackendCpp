#include "dog.h"

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

void Dog::SetPosition(Position position) {
    position_ = std::move(position);
};

const Position& Dog::GetPosition() const {
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

Position Dog::CalculateNewPosition(const std::chrono::milliseconds& delta_time) {
    Position position = GetPosition();
    const Velocity& velocity = GetVelocity();
    position.x += velocity.vx * delta_time.count()/1000;
    position.y += velocity.vy * delta_time.count()/1000;
    return position;
};

}