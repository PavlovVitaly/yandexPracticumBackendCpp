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
        const app::Application& application) {
    return req.target() == "/api/v1/maps" || req.target() == "/api/v1/maps/";
}

template <typename Request, typename Send>
void GetMapListHandler(
        const Request& req,
        const app::Application& application,
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
        const app::Application& application) {
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
        const app::Application& application,
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
        const app::Application& application) {
    auto url = SplitUrl(req.target());
    return !url.empty() &&
            url[0] == "api" &&
            (
                url.size() > SIZE_OF_FOUR_SEGMENT_URL ||
                url.size() < SIZE_OF_THREE_SEGMENT_URL ||
                (url.size() >= SIZE_OF_TWO_SEGMENT_URL && 
                    url[1] != "v1") ||
                (url.size() >= SIZE_OF_THREE_SEGMENT_URL &&
                    url[2] != "maps")
            );
};

template <typename Request, typename Send>
void BadRequestHandler(
        const Request& req,
        const app::Application& application,
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
        const app::Application& application) {
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
        const app::Application& application,
        Send&& send)                                                                                                                                                                          {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::CreateMapNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
};

template <typename Request, typename Send>
void PageNotFoundHandler(
        const Request& req,
        const app::Application& application,
        Send&& send) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::CreatePageNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
};

}