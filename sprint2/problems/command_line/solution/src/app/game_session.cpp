#include "game_session.h"
#include "random_generators.h"
#include "support_types.h"

namespace app {

const GameSession::Id& GameSession::GetId() const noexcept {
    return id_;
}

const std::shared_ptr<model::Map> GameSession::GetMap() {
    return map_;
};

std::shared_ptr<GameSession::SessionStrand> GameSession::GetStrand() {
    return strand_;
};

}