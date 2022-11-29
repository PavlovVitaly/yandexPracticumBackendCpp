#pragma once
#include "tagged.h"
#include "game_session.h"

#include <string>

namespace model {

class Player {
    inline static size_t max_id_cont_ = 0;
public:
    using Id = util::Tagged<size_t, Player>;
    Player(std::string name) : 
        id_(Id{Player::max_id_cont_++}),
        name_(name) {};
    Player(Id id, std::string name) :
        id_(id),
        name_(name) {};
    Player(const Player& other) = default;
    Player(Player&& other) = default;
    Player& operator = (const Player& other) = default;
    Player& operator = (Player&& other) = default;
    virtual ~Player() = default;

    const Id& GetId() const;
    const std::string& GetName() const;
    const model::GameSession::Id& GetGameSessionId() const;
    std::weak_ptr<GameSession> GetGameSession();
    void SetGameSession(std::weak_ptr<GameSession> session);
    std::weak_ptr<Dog> GetDog();
    void SetDog(std::weak_ptr<Dog> dog);
    void MoveDog(const std::chrono::milliseconds& delta_time);
private:
    Id id_;
    std::string name_;
    std::weak_ptr<GameSession> session_;
    std::weak_ptr<Dog> dog_;
};

}