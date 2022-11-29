#pragma once
#include "map.h"
#include "game_session.h"
#include "player.h"

#include <memory>
#include <chrono>

namespace model {

class Game {
public:
    using Maps = std::vector< std::shared_ptr<Map> >;

    void AddMap(Map map);
    void AddMaps(const std::vector<Map>& maps);
    const Maps& GetMaps() const noexcept;
    const std::shared_ptr<Map> FindMap(const Map::Id& id) const noexcept;
    void AddGameSession(std::shared_ptr<GameSession> session);
    std::shared_ptr<GameSession> FindGameSessionBy(const Map::Id& id) const noexcept;
    void SetDefaultDogVelocity(double velocity);
    double GetDefaultDogVelocity() const noexcept;

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;
    using MapIdToSessionIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

    std::vector< std::shared_ptr<Map> > maps_;
    MapIdToIndex map_id_to_index_;
    std::vector< std::shared_ptr<GameSession> > sessions_;
    MapIdToSessionIndex map_id_to_session_index_;
    double default_dog_velocity_{INITIAL_DOG_VELOCITY};
};

}