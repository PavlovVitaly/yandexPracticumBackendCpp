#include "player_serialization.h"

namespace game_data_ser {

app::Player PlayerSerialization::Restore() const {
    return app::Player(app::Player::Id{id_}, name_);
};

model::Dog PlayerSerialization::RestoreDog() const {
    return dog_ser_.Restore();
};

authentication::Token PlayerSerialization::RestoreToken() const {
    return authentication::Token(token_);
};

}