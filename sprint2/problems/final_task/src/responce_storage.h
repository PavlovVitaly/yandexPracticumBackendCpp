/*
# Условия в активаторах выглядят так себе, но мне кажется это несколько проще для восприятия, чем при использовании регулярок.
*/
#pragma once
#include "model.h"
#include "json_converter.h"
#include <vector>
#include <boost/beast/http.hpp>

namespace responce_storage{

namespace beast = boost::beast;
namespace http = beast::http;
using StringResponse = http::response<http::string_body>;

const size_t SIZE_OF_TWO_SEGMENT_URL = 2;
const size_t SIZE_OF_THREE_SEGMENT_URL = 3;
const size_t SIZE_OF_FOUR_SEGMENT_URL = 4;

std::vector<std::string_view> SplitUrl(std::string_view str);

template <typename Body, typename Allocator>
bool UseGetMapListActivator(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game) {
    return req.target() == "/api/v1/maps" || req.target() == "/api/v1/maps/";
}

template <typename Body, typename Allocator>
StringResponse MakeGetMapListResponse(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game) {
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::ConvertMapListToJson(game);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
}

template <typename Body, typename Allocator>
bool UseGetMapByIdActivator(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game) {
    auto url = SplitUrl(req.target());
    return url.size() == SIZE_OF_FOUR_SEGMENT_URL &&
            url[0] == "api" &&
            url[1] == "v1" &&
            url[2] == "maps" &&
            game.FindMap(model::Map::Id(std::string(url[3]))) != nullptr;
}

template <typename Body, typename Allocator>
StringResponse MakeGetMapByIdResponse(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game) {
    http::response<http::string_body> response(http::status::ok, req.version());
    auto id = SplitUrl(req.target())[3];
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::ConvertMapToJson(*game.FindMap(model::Map::Id(std::string(id))));
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
};


template <typename Body, typename Allocator>
bool UseBadRequestActivator(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game) {
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

template <typename Body, typename Allocator>
StringResponse MakeBadRequestResponse(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::CreateBadRequestResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
};

template <typename Body, typename Allocator>
bool UseMapNotFoundActivator(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game) {
    auto url = SplitUrl(req.target());
    return url.size() == SIZE_OF_FOUR_SEGMENT_URL &&
            url[0] == "api" &&
            url[1] == "v1" &&
            url[2] == "maps" &&
            game.FindMap(model::Map::Id(std::string(url[3]))) == nullptr;
};

template <typename Body, typename Allocator>
StringResponse MakeMapNotFoundResponse(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game)                                                                                                                                                                          {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::CreateMapNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
};

template <typename Body, typename Allocator>
StringResponse MakePageNotFoundResponse(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = json_converter::CreatePageNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
};

}