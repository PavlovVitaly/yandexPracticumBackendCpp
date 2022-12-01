#pragma once
#include "application.h"
#include "player_tokens.h"
#include "json_converter.h"
#include "request_handlers_utils.h"

#include <vector>
#include <optional>
#include <boost/beast/http.hpp>

#include <chrono> //todo: for testing

namespace rh_storage{

namespace beast = boost::beast;
namespace http = beast::http;
using StringResponse = http::response<http::string_body>;

const size_t SIZE_OF_TWO_SEGMENT_URL = 2;
const size_t SIZE_OF_THREE_SEGMENT_URL = 3;
const size_t SIZE_OF_FOUR_SEGMENT_URL = 4;
const size_t SIZE_OF_FIVE_SEGMENT_URL = 5;

template <typename Request>
bool BadRequestActivator(const Request& req) {
    auto url = SplitUrl(req.target());
    return !url.empty() &&
            url[0] == "api" &&
            (
                url.size() > SIZE_OF_FIVE_SEGMENT_URL ||
                url.size() < SIZE_OF_THREE_SEGMENT_URL ||
                (url.size() >= SIZE_OF_TWO_SEGMENT_URL && 
                    url[1] != "v1") ||
                (url.size() >= SIZE_OF_THREE_SEGMENT_URL &&
                    url[2] != "maps" &&
                    url[2] != "game" &&
                    url[3] != "join" &&
                    url[3] != "players" &&
                    url[3] != "state" &&
                    url[3] != "player" &&
                    url[3] != "tick" &&     // todo: temp url
                    (url.size() == SIZE_OF_FIVE_SEGMENT_URL && url[4] != "action"))
            );
};

template <typename Request, typename Send>
std::optional<size_t> BadRequestHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::CreateBadRequestResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};


template <typename Request>
bool GetMapListActivator(const Request& req) {
    return req.target() == "/api/v1/maps" || req.target() == "/api/v1/maps/";
}

template <typename Request, typename Send>
std::optional<size_t> GetMapListHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::ConvertMapListToJson(application.ListMap());
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request>
bool GetMapByIdActivator(const Request& req) {
    auto url = SplitUrl(req.target());
    return url.size() == SIZE_OF_FOUR_SEGMENT_URL &&
            url[0] == "api" &&
            url[1] == "v1" &&
            url[2] == "maps";
}

template <typename Request, typename Send>
std::optional<size_t> GetMapByIdHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    auto id = SplitUrl(req.target())[3];
    auto map = application.FindMap(model::Map::Id(std::string(id)));
    if(map == nullptr) {
        return 0;
    }
    http::response<http::string_body> response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::ConvertMapToJson(*map);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};

template <typename Request, typename Send>
std::optional<size_t> MapNotFoundHandler(
        const Request& req,
        app::Application& application,
        Send& send)                                                                                                                                                                          {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::CreateMapNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};


template <typename Request>
bool JoinToGameInvalidJsonReqActivator(const Request& req) {
    return (req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/") &&
        !json_converter::ParseJoinToGameRequest(req.body());
}

template <typename Request, typename Send>
std::optional<size_t> JoinToGameInvalidJsonReqHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateJoinToGameInvalidArgumentResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request>
bool JoinToGameEmptyPlayerNameActivator(const Request& req) {
    if((req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/")) {
        auto res = json_converter::ParseJoinToGameRequest(req.body());
        if(!res) {
            return false;
        }
        auto [player_name, map_id] = res.value();
        return player_name.empty();
    }
    return false;
}

template <typename Request, typename Send>
std::optional<size_t> JoinToGameEmptyPlayerNameHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateJoinToGameEmptyPlayerNameResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request>
bool JoinToGameActivator(const Request& req) {
    return (req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/");
}

template <typename Request, typename Send>
std::optional<size_t> JoinToGameHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    auto [player_name, map_id] = json_converter::ParseJoinToGameRequest(req.body()).value();
    if(application.FindMap(map_id) == nullptr) {
        return 0;
    }
    auto [token, player_id] = application.JoinGame(player_name, map_id);
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateJoinToGameResponse(*token, *player_id);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request, typename Send>
std::optional<size_t> JoinToGameMapNotFoundHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateJoinToGameMapNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request, typename Send>
std::optional<size_t> OnlyPostMethodAllowedHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::method_not_allowed, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.set(http::field::allow, "POST");
    response.body() = json_converter::CreateOnlyPostMethodAllowedResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};


template <typename Request>
bool EmptyAuthorizationActivator(const Request& req) {
    return ((req.target() == "/api/v1/game/players" ||
            req.target() == "/api/v1/game/players/") ||
            (req.target() == "/api/v1/game/state" ||
            req.target() == "/api/v1/game/state/") ||
            (req.target() == "/api/v1/game/player/action" ||
            req.target() == "/api/v1/game/player/action/")) && // todo: need rework
            (req[http::field::authorization].empty() ||
            GetTokenString(req[http::field::authorization]).empty());
}

template <typename Request, typename Send>
std::optional<size_t> EmptyAuthorizationHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::unauthorized, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateEmptyAuthorizationResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request>
bool GetPlayersListActivator(const Request& req) {
    return (req.target() == "/api/v1/game/players" || req.target() == "/api/v1/game/players/");
}

template <typename Request, typename Send>
std::optional<size_t> GetPlayersListHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    authentication::Token token{GetTokenString(req[http::field::authorization])};
    if(!application.IsExistPlayer(token)) {
        return 0;
    }
    auto players = application.GetPlayersFromGameSession(token);
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreatePlayersListOnMapResponse(players);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request, typename Send>
std::optional<size_t> InvalidMethodHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::method_not_allowed, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.set(http::field::allow, "GET, HEAD");
    response.body() = json_converter::CreateInvalidMethodResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};


template <typename Request>
bool GetGameStateActivator(const Request& req) {
    return (req.target() == "/api/v1/game/state" || req.target() == "/api/v1/game/state/");
}

template <typename Request, typename Send>
std::optional<size_t> GetGameStateHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    authentication::Token token{GetTokenString(req[http::field::authorization])};
    if(!application.IsExistPlayer(token)) {
        return 0;
    }
    auto players = application.GetPlayersFromGameSession(token);
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateGameStateResponse(players);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request>
bool InvalidContentTypeActivator(const Request& req) {
    return ((req.target() == "/api/v1/game/join" ||
            req.target() == "/api/v1/game/join/") ||
            (req.target() == "/api/v1/game/player/action" ||
            req.target() == "/api/v1/game/player/action/")) && // todo: need rework
            (req[http::field::content_type].empty() ||
            req[http::field::content_type] != "application/json");
}

template <typename Request, typename Send>
std::optional<size_t> InvalidContentTypeHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateInvalidContentTypeResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request>
bool PlayerActionInvalidActionActivator(const Request& req) {
    if((req.target() == "/api/v1/game/player/action" || req.target() == "/api/v1/game/player/action/")) {
        auto res = json_converter::ParsePlayerActionRequest(req.body());
        if(res.has_value()) {
            return !model::STRING_TO_DIRECTION.contains(res.value());
        }
    }
    return false;
}

template <typename Request, typename Send>
std::optional<size_t> PlayerActionInvalidActionHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreatePlayerActionInvalidActionResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request>
bool PlayerActionActivator(const Request& req) {
    return (req.target() == "/api/v1/game/player/action" || req.target() == "/api/v1/game/player/action");
}

template <typename Request, typename Send>
std::optional<size_t> PlayerActionHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    authentication::Token token{GetTokenString(req[http::field::authorization])};
    if(!application.IsExistPlayer(token)) {
        return 0;
    }
    std::string directionStr = json_converter::ParsePlayerActionRequest(req.body()).value();
    application.SetPlayerAction(token, model::STRING_TO_DIRECTION.at(directionStr));
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreatePlayerActionResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request, typename Send>
std::optional<size_t> UnknownTokenHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::unauthorized, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateUnknownTokenResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request, typename Send>
std::optional<size_t> PageNotFoundHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::CreatePageNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};


template <typename Request>
bool TimeTickInvalidMsgActivator(const Request& req) {
    if((req.target() == "/api/v1/game/tick" || req.target() == "/api/v1/game/tick/")) {
        auto res = json_converter::ParseSetDeltaTimeRequest(req.body());
        return !res.has_value();
    }
    return false;
}

template <typename Request, typename Send>
std::optional<size_t> TimeTickInvalidMsgHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    if(!application.IsManualTimeManagement()) {
        return 0;
    }
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateSetDeltaTimeInvalidMsgResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request>
bool TimeTickActivator(const Request& req) {
    return (req.target() == "/api/v1/game/tick" || req.target() == "/api/v1/game/tick");
}

template <typename Request, typename Send>
std::optional<size_t> TimeTickHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    if(!application.IsManualTimeManagement()) {
        return 0;
    }
    int delta_time = json_converter::ParseSetDeltaTimeRequest(req.body()).value();
    std::chrono::milliseconds dtime(delta_time);
    application.UpdateGameState(dtime);
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateSetDeltaTimeResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request, typename Send>
std::optional<size_t> InvalidEndpointHandler(
        const Request& req,
        app::Application& application,
        Send& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateInvalidEndpointResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

}