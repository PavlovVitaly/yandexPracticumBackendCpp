#pragma once

#include <string>

namespace api_urls {

const std::string GET_PLAYERS_LIST_API = "/api/v1/game/players";
const std::string GET_GAME_STATE_API = "/api/v1/game/state";
const std::string MAKE_ACTION_API = "/api/v1/game/player/action";
const std::string JOIN_TO_GAME_API = "/api/v1/game/join";
const std::string GET_MAPS_LIST_API = "/api/v1/maps";
const std::string MAKE_TIME_TICK_API = "/api/v1/game/tick";
const std::string GET_RECORDS_API = "/api/v1/game/records";

}