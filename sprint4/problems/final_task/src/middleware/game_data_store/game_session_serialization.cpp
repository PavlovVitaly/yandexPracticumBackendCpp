#include "game_session_serialization.h"

namespace game_data_ser {

model::Map::Id GameSessionSerialization::RestoreMapId() const {
    return model::Map::Id(map_id_);
};

const std::vector<LostObjectSerialization>& GameSessionSerialization::GetLostObjectsSerialize() const {
    return lost_objects_;
};

const std::vector<PlayerSerialization>& GameSessionSerialization::GetPlayersSerialize() const {
    return players_ser_;
};

}