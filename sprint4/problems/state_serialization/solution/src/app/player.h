#pragma once
#include "tagged.h"
#include "game_session.h"

#include <string>

namespace app {

class Player {
    inline static size_t max_id_cont_ = 0;
public:
    using Id = util::Tagged<size_t, Player>;
    Player(std::string name) : 
        id_(Id{Player::max_id_cont_++}),
        name_(name) {};
    Player(Id id, std::string name) :
        id_(id),
        name_(name) {
        if(*id_ >= Player::max_id_cont_){
            Player::max_id_cont_ = *id_ + 1;
        }
    };
    Player(const Player& other) = default;
    Player(Player&& other) = default;
    Player& operator = (const Player& other) = default;
    Player& operator = (Player&& other) = default;
    virtual ~Player() = default;

    const Id& GetId() const;
    const std::string& GetName() const;
    const GameSession::Id& GetGameSessionId() const;
    std::shared_ptr<GameSession> GetGameSession();
    void SetGameSession(std::shared_ptr<GameSession> session);
    std::weak_ptr<model::Dog> GetDog();
    void SetDog(std::weak_ptr<model::Dog> dog);
private:
    Id id_;
    std::string name_;
    std::shared_ptr<GameSession> session_;
    std::weak_ptr<model::Dog> dog_;
};

}