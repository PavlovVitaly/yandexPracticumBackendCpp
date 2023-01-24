#pragma once
#include "application.h"
#include "token_generator.h"
#include "json_converter.h"
#include "request_handlers_utils.h"
#include "api_url_storage.h"
#include "url_invariants.h"

#include <vector>
#include <optional>
#include <variant>
#include <chrono>
#include <unordered_set>
#include <sstream>
#include <boost/beast/http.hpp>
#include <boost/thread/future.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/parse.hpp>

namespace rh_storage{

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using StringResponse = http::response<http::string_body>;

const size_t SIZE_OF_TWO_SEGMENT_URL = 2;
const size_t SIZE_OF_THREE_SEGMENT_URL = 3;
const size_t SIZE_OF_FOUR_SEGMENT_URL = 4;
const size_t SIZE_OF_FIVE_SEGMENT_URL = 5;

const std::unordered_set<std::string_view> GAME_API_URLS_WITH_AUTHORIZATION = {
    api_urls::GET_PLAYERS_LIST_API,
    api_urls::GET_PLAYERS_LIST_API + "/",
    api_urls::GET_GAME_STATE_API,
    api_urls::GET_GAME_STATE_API + "/",
    api_urls::MAKE_ACTION_API,
    api_urls::MAKE_ACTION_API + "/",
    api_urls::GET_RECORDS_API,
    api_urls::GET_RECORDS_API + "/"
};

const std::unordered_set<std::string_view> GAME_API_URLS_WITH_JSON_REQ = {
    api_urls::JOIN_TO_GAME_API,
    api_urls::JOIN_TO_GAME_API + "/",
    api_urls::MAKE_ACTION_API,
    api_urls::MAKE_ACTION_API + "/"
};

const std::string CONTENT_TYPE_APPLICATION_JSON = "application/json";
const std::string NO_CACHE_CONTROL = "no-cache";


template <typename Request>
bool BadRequestActivator(const Request& req) {
    auto url = SplitUrl(boost::urls::url_view{req.target()}.path());
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
                    url[3] != "tick" &&
                    (url.size() == SIZE_OF_FIVE_SEGMENT_URL && url[4] != "action"))
            ); // todo: need refactor
};

template <typename Request, typename Send>
std::optional<size_t> BadRequestHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.body() = json_converter::CreateBadRequestResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};


template <typename Request>
bool GetMapListActivator(const Request& req) {
    return IsEqualUrls(api_urls::GET_MAPS_LIST_API, req.target());
}

template <typename Request, typename Send>
std::optional<size_t> GetMapListHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::ConvertMapListToJson(application->ListMap());
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
        std::shared_ptr<app::Application> application,
        Send&& send) {
    auto id = SplitUrl(req.target())[3];
    auto map = application->FindMap(model::Map::Id(std::string(id)));
    if(!map) {
        return 0;
    }
    http::response<http::string_body> response(http::status::ok, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::ConvertMapToJson(**map);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};

template <typename Request, typename Send>
std::optional<size_t> MapNotFoundHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send)                                                                                                                                                                          {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreateMapNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};


template <typename Request>
bool JoinToGameInvalidJsonReqActivator(const Request& req) {
    return IsEqualUrls(api_urls::JOIN_TO_GAME_API, req.target()) &&
        !json_converter::ParseJoinToGameRequest(req.body());
}

template <typename Request, typename Send>
std::optional<size_t> JoinToGameInvalidJsonReqHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreateJoinToGameInvalidArgumentResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request>
bool JoinToGameEmptyPlayerNameActivator(const Request& req) {
    if(IsEqualUrls(api_urls::JOIN_TO_GAME_API, req.target())) {
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
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreateJoinToGameEmptyPlayerNameResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request>
bool JoinToGameActivator(const Request& req) {
    return IsEqualUrls(api_urls::JOIN_TO_GAME_API, req.target());
}

template <typename Request, typename Send>
std::optional<size_t> JoinToGameHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    auto [player_name, map_id] = json_converter::ParseJoinToGameRequest(req.body()).value();
    if(!application->FindMap(map_id)) {
        return 0;
    }
    StringResponse response(http::status::ok, req.version());
    auto session = application->FindGameSessionBy(map_id);
    if(session) {
        boost::promise<std::string> res_promise;
        auto res_future = res_promise.get_future();
        net::dispatch(*(session.value()->GetStrand()),
            [&res_promise
            , application
            , &player_name
            , &map_id] {
            auto [token, player_id] = application->JoinGame(player_name, map_id);
            res_promise.set_value(json_converter::CreateJoinToGameResponse(*token, *player_id));
        });
        response.body() = res_future.get();
    } else {
        auto [token, player_id] = application->JoinGame(player_name, map_id);
        response.body() = json_converter::CreateJoinToGameResponse(*token, *player_id);
    }
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request, typename Send>
std::optional<size_t> JoinToGameMapNotFoundHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreateJoinToGameMapNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request, typename Send>
std::optional<size_t> OnlyPostMethodAllowedHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::method_not_allowed, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.set(http::field::allow, "POST");
    response.body() = json_converter::CreateOnlyPostMethodAllowedResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};


template <typename Request>
bool EmptyAuthorizationActivator(const Request& req) {
    return (GAME_API_URLS_WITH_AUTHORIZATION.count(req.target()) > 0) &&
            (req[http::field::authorization].empty() ||
            GetTokenString(req[http::field::authorization]).empty());
}

template <typename Request, typename Send>
std::optional<size_t> EmptyAuthorizationHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::unauthorized, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreateEmptyAuthorizationResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request>
bool GetPlayersListActivator(const Request& req) {
    return IsEqualUrls(api_urls::GET_PLAYERS_LIST_API, req.target());
}

template <typename Request, typename Send>
std::optional<size_t> GetPlayersListHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    authentication::Token token{GetTokenString(req[http::field::authorization])};
    if(!application->IsExistPlayer(token)) {
        return 0;
    }
    boost::promise<std::variant<std::string, size_t>> res_promise;
    auto res_future = res_promise.get_future();
    net::dispatch(*(application->FindGameSessionBy(token).value()->GetStrand()),
        [&res_promise
        , &token
        , application]{
        res_promise.set_value(
            json_converter::CreatePlayersListOnMapResponse(application->GetPlayersFromGameSession(token))
        );
    });
    auto res = res_future.get();
    if(std::holds_alternative<size_t>(res)){
        return std::get<size_t>(res);
    }
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = std::get<std::string>(res);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request, typename Send>
std::optional<size_t> InvalidMethodHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::method_not_allowed, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.set(http::field::allow, "GET, HEAD");
    response.body() = json_converter::CreateInvalidMethodResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};


template <typename Request>
bool GetGameStateActivator(const Request& req) {
    return IsEqualUrls(api_urls::GET_GAME_STATE_API, req.target());
}

template <typename Request, typename Send>
std::optional<size_t> GetGameStateHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    authentication::Token token{GetTokenString(req[http::field::authorization])};
    if(!application->IsExistPlayer(token)) {
        return 0;
    }
    
    boost::promise<std::variant<std::string, size_t>> res_promise;
    auto res_future = res_promise.get_future();
    net::dispatch(*(application->FindGameSessionBy(token).value()->GetStrand()),
        [&res_promise
        , &token
        , application] {
        auto session = application->FindGameSessionBy(token);
        if(!session) {
            res_promise.set_value(0ul);
            return;    
        }
        res_promise.set_value(json_converter::CreateGameStateResponse(
            application->GetPlayersFromGameSession(token),
            session.value()->GetLostObjects())
        );
    });
    auto res = res_future.get();
    if(std::holds_alternative<size_t>(res)){
        return std::get<size_t>(res);
    }
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = std::get<std::string>(res);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request>
bool InvalidContentTypeActivator(const Request& req) {
    return (GAME_API_URLS_WITH_JSON_REQ.count(req.target()) > 0) &&
            (req[http::field::content_type].empty() ||
            req[http::field::content_type] != CONTENT_TYPE_APPLICATION_JSON);
}

template <typename Request, typename Send>
std::optional<size_t> InvalidContentTypeHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreateInvalidContentTypeResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request>
bool PlayerActionInvalidActionActivator(const Request& req) {
    if(IsEqualUrls(api_urls::MAKE_ACTION_API, req.target())) {
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
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreatePlayerActionInvalidActionResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request>
bool PlayerActionActivator(const Request& req) {
    return IsEqualUrls(api_urls::MAKE_ACTION_API, req.target());
}

template <typename Request, typename Send>
std::optional<size_t> PlayerActionHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    authentication::Token token{GetTokenString(req[http::field::authorization])};
    if(!application->IsExistPlayer(token)) {
        return 0;
    }
    boost::promise<std::optional<size_t>> res_promise;
    auto res_future = res_promise.get_future();
    net::dispatch(*(application->FindGameSessionBy(token).value()->GetStrand()),
        [&res_promise
        , &token
        , &req
        , application]{
        std::string directionStr = json_converter::ParsePlayerActionRequest(req.body()).value();
        application->SetPlayerAction(token, model::STRING_TO_DIRECTION.at(directionStr));
        res_promise.set_value(std::nullopt);
    });
    auto res = res_future.get();
    if(res) {
        return res;
    }
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreatePlayerActionResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return res;
}


template <typename Request, typename Send>
std::optional<size_t> UnknownTokenHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::unauthorized, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreateUnknownTokenResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request, typename Send>
std::optional<size_t> PageNotFoundHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.body() = json_converter::CreatePageNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
};


template <typename Request>
bool TimeTickInvalidMsgActivator(const Request& req) {
    if(IsEqualUrls(api_urls::MAKE_TIME_TICK_API, req.target())) {
        auto res = json_converter::ParseSetDeltaTimeRequest(req.body());
        return !res.has_value();
    }
    return false;
}

template <typename Request, typename Send>
std::optional<size_t> TimeTickInvalidMsgHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    if(!application->IsManualTimeManagement()) {
        return 0;
    }
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreateSetDeltaTimeInvalidMsgResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}


template <typename Request>
bool TimeTickActivator(const Request& req) {
    return IsEqualUrls(api_urls::MAKE_TIME_TICK_API, req.target());
}

template <typename Request, typename Send>
std::optional<size_t> TimeTickHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    if(!application->IsManualTimeManagement()) {
        return 0;
    }
    std::chrono::milliseconds dtime(json_converter::ParseSetDeltaTimeRequest(req.body()).value());
    application->UpdateGameState(dtime);
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreateSetDeltaTimeResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request, typename Send>
std::optional<size_t> InvalidEndpointHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreateInvalidEndpointResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

template <typename Request>
bool GetRecordsActivator(const Request& req) {
    return IsEqualUrls(api_urls::GET_RECORDS_API, boost::urls::url_view{req.target()}.path());
}

template <typename Request, typename Send>
std::optional<size_t> GetRecordsHandler(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
    std::optional<size_t> offset;
    std::optional<size_t> limit;
    auto params = boost::urls::url_view{req.target()}.params();

    if(params.contains(url_invariants::URL_PARAMETER_START)){
        offset = GetValueFromUrlParameter<size_t>(params, url_invariants::URL_PARAMETER_START);
    }

    if(params.contains(url_invariants::URL_PARAMETER_MAX_ITEMS)){
        limit = GetValueFromUrlParameter<size_t>(params, url_invariants::URL_PARAMETER_MAX_ITEMS);
    }
    
    auto records_table = application->GetRecordsTable(offset, limit);
    if(!records_table) {
        return 0 ;    
    }
    
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, CONTENT_TYPE_APPLICATION_JSON);
    response.set(http::field::cache_control, NO_CACHE_CONTROL);
    response.body() = json_converter::CreateRecordsTableResponse(*records_table);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
    return std::nullopt;
}

}