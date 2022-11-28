#pragma once
#include "application.h"
#include "player_tokens.h"
#include "json_converter.h"
#include "request_handlers_utility.h"

#include <vector>
#include <boost/beast/http.hpp>

namespace rh_storage{

namespace beast = boost::beast;
namespace http = beast::http;
using StringResponse = http::response<http::string_body>;

const size_t SIZE_OF_TWO_SEGMENT_URL = 2;
const size_t SIZE_OF_THREE_SEGMENT_URL = 3;
const size_t SIZE_OF_FOUR_SEGMENT_URL = 4;


template <typename Request>
bool GetMapListActivator(
        const Request& req,
        app::Application& application) {
    return req.target() == "/api/v1/maps" || req.target() == "/api/v1/maps/";
}

template <typename Request, typename Send>
void GetMapListHandler(
        const Request& req,
        app::Application& application,
        Send&& send) {
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::ConvertMapListToJson(application.ListMap());
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
}


template <typename Request>
bool GetMapByIdActivator(
        const Request& req,
        app::Application& application) {
    auto url = SplitUrl(req.target());
    return url.size() == SIZE_OF_FOUR_SEGMENT_URL &&
            url[0] == "api" &&
            url[1] == "v1" &&
            url[2] == "maps" &&
            application.FindMap(model::Map::Id(std::string(url[3]))) != nullptr;
}

template <typename Request, typename Send>
void GetMapByIdHandler(
        const Request& req,
        app::Application& application,
        Send&& send) {
    http::response<http::string_body> response(http::status::ok, req.version());
    auto id = SplitUrl(req.target())[3];
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::ConvertMapToJson(*application.FindMap(model::Map::Id(std::string(id))));
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
};


template <typename Request>
bool BadRequestActivator(
        const Request& req,
        app::Application& application) {
    auto url = SplitUrl(req.target());
    return !url.empty() &&
            url[0] == "api" &&
            (
                url.size() > SIZE_OF_FOUR_SEGMENT_URL ||
                url.size() < SIZE_OF_THREE_SEGMENT_URL ||
                (url.size() >= SIZE_OF_TWO_SEGMENT_URL && 
                    url[1] != "v1") ||
                (url.size() >= SIZE_OF_THREE_SEGMENT_URL &&
                    url[2] != "maps" &&
                    url[2] != "game" &&
                    url[3] != "join")
            );
};

template <typename Request, typename Send>
void BadRequestHandler(
        const Request& req,
        app::Application& application,
        Send&& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::CreateBadRequestResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
};


template <typename Request>
bool MapNotFoundActivator(
        const Request& req,
        app::Application& application) {
    auto url = SplitUrl(req.target());
    return url.size() == SIZE_OF_FOUR_SEGMENT_URL &&
            url[0] == "api" &&
            url[1] == "v1" &&
            url[2] == "maps" &&
            application.FindMap(model::Map::Id(std::string(url[3]))) == nullptr;
};

template <typename Request, typename Send>
void MapNotFoundHandler(
        const Request& req,
        app::Application& application,
        Send&& send)                                                                                                                                                                          {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::CreateMapNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
};


template <typename Request>
bool JoinToGameInvalidJsonReqActivator(
        const Request& req,
        app::Application& application) {
    return (req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/") &&
        !json_converter::ParseJoinToGameRequest(req.body());
}

template <typename Request, typename Send>
void JoinToGameInvalidJsonReqHandler(
        const Request& req,
        app::Application& application,
        Send&& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateJoinToGameInvalidArgumentResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
}


template <typename Request>
bool JoinToGameMapNotFoundActivator(
        const Request& req,
        app::Application& application) {
    if((req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/")) {
        auto res = json_converter::ParseJoinToGameRequest(req.body());
        if(!res) {
            return false;
        }
        auto [player_name, map_id] = res.value();
        return application.FindMap(map_id) == nullptr;
    }
    return false;
}

template <typename Request, typename Send>
void JoinToGameMapNotFoundHandler(
        const Request& req,
        app::Application& application,
        Send&& send) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateJoinToGameMapNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
}


template <typename Request>
bool JoinToGameEmptyPlayerNameActivator(
        const Request& req,
        app::Application& application) {
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
void JoinToGameEmptyPlayerNameHandler(
        const Request& req,
        app::Application& application,
        Send&& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateJoinToGameEmptyPlayerNameResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
}


template <typename Request>
bool JoinToGameActivator(
        const Request& req,
        app::Application& application) {
    return (req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/");
}

template <typename Request, typename Send>
void JoinToGameHandler(
        const Request& req,
        app::Application& application,
        Send&& send) {
    auto [player_name, map_id] = json_converter::ParseJoinToGameRequest(req.body()).value();
    auto [token, player_id] = application.JoinGame(player_name, map_id);
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.body() = json_converter::CreateJoinToGameResponse(*token, *player_id);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
}

template <typename Request, typename Send>
void OnlyPostMethodAllowedHandler(
        const Request& req,
        app::Application& application,
        Send&& send) {
    StringResponse response(http::status::method_not_allowed, req.version());
    response.set(http::field::content_type, "application/json");
    response.set(http::field::cache_control, "no-cache");
    response.set(http::field::allow, "POST");
    response.body() = json_converter::CreateOnlyPostMethodAllowedResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
};

template <typename Request, typename Send>
void PageNotFoundHandler(
        const Request& req,
        app::Application& application,
        Send&& send) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::CreatePageNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
};

}