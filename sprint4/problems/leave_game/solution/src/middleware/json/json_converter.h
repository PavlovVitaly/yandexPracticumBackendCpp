#pragma once
#include "application.h"
#include "player_record.h"

#include <string>
#include <tuple>
#include <optional>

namespace json_converter{

std::string ConvertMapListToJson(const model::Game::Maps& maps);
std::string ConvertMapToJson(const model::Map& map);
std::string CreateMapNotFoundResponse();
std::string CreateBadRequestResponse();
std::string CreatePageNotFoundResponse();
std::string CreateOnlyPostMethodAllowedResponse();
std::string CreateJoinToGameInvalidArgumentResponse();
std::string CreateJoinToGameMapNotFoundResponse();
std::string CreateJoinToGameEmptyPlayerNameResponse();
std::string CreateInvalidMethodResponse();
std::string CreateEmptyAuthorizationResponse();
std::string CreateUnknownTokenResponse();
std::string CreatePlayerActionResponse();
std::string CreatePlayerActionInvalidActionResponse();
std::string CreateInvalidContentTypeResponse();
std::string CreatePlayersListOnMapResponse(const std::vector< std::shared_ptr<app::Player> >& players);
std::string CreateGameStateResponse(const std::vector< std::shared_ptr<app::Player> >& players,
                                    const app::GameSession::LostObjects& lost_objects);
std::string CreateSetDeltaTimeResponse();
std::string CreateSetDeltaTimeInvalidMsgResponse();
std::string CreateInvalidEndpointResponse();
std::string CreateRecordsTableResponse(const std::vector<domain::PlayerRecord>& records_table);

std::string CreateJoinToGameResponse(const std::string& token, size_t player_id);
std::optional< std::tuple<std::string, model::Map::Id> > ParseJoinToGameRequest(const std::string& msg);
std::optional<std::string> ParsePlayerActionRequest(const std::string& msg);
std::optional<int> ParseSetDeltaTimeRequest(const std::string& msg);
}