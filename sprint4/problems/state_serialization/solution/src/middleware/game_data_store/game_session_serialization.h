#pragma once
#include "game_session.h"
#include "player_serialization.h"
#include "map.h"
#include "player_tokens.h"
#include "lost_object_serialization.h"

#include <vector>
#include <boost/serialization/vector.hpp>

namespace game_data_ser {

class GameSessionSerialization {
public:
    GameSessionSerialization(
        app::GameSession& game_session,
        const std::unordered_map< authentication::Token, std::shared_ptr<app::Player>,
                                    authentication::TokenHasher >& tokenToPalyer)
        : map_id_(*(game_session.GetMap()->GetId())) {
        std::ranges::transform(tokenToPalyer, std::back_inserter(players_ser_),
            [](const auto& token_to_player)->PlayerSerialization {
                return PlayerSerialization(*token_to_player.second, token_to_player.first);
            }
        );
    };

    [[nodiscard]] model::Map::Id RestoreMapId() const;
    [[nodiscard]] const std::vector<PlayerSerialization>& GetPlayersSerialize() const;

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar& map_id_;
        ar& players_ser_;

    }
private:
    std::string map_id_;
    std::vector<PlayerSerialization> players_ser_;
    std::vector<LostObjectSerialization> lost_object_;
};

}