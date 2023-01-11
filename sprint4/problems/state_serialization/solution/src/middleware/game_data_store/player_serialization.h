#pragma once
#include "player.h"
#include "dog_serialization.h"
#include "player_tokens.h"

#include <boost/serialization/vector.hpp>

namespace game_data_ser {

class PlayerSerialization {
public:
    PlayerSerialization(app::Player& player, const authentication::Token& token):
        id_(*player.GetId()),
        name_(player.GetName()),
        dog_ser_(*player.GetDog().lock()),
        token_(*token) {};

    [[nodiscard]] app::Player Restore() const;
    [[nodiscard]] model::Dog RestoreDog() const;
    [[nodiscard]] authentication::Token RestoreToken() const;

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar& id_;
        ar& name_;
        ar& dog_ser_;
    }
private:
    size_t id_;
    std::string name_;
    DogSerialization dog_ser_;
    std::string token_;
};

}