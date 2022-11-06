#pragma once
#include "model.h"
#include "json_converter.h"
#include <vector>
#include <boost/beast/http.hpp>

namespace responceStorage{

namespace beast = boost::beast;
namespace http = beast::http;
using StringResponse = http::response<http::string_body>;

std::vector<std::string_view> SplitUrl(std::string_view str);

template <typename Body, typename Allocator>
bool MakeGetMapListResponseActivator(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game){
    return req.target() == "/api/v1/maps" or req.target() == "/api/v1/maps/";
}

template <typename Body, typename Allocator>
StringResponse MakeGetMapListResponse(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game){
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = jsonConverter::ConvertMapListToJson(game);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
}

template <typename Body, typename Allocator>
bool MakeGetMapByIdResponseActivator(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game){
    auto url = SplitUrl(req.target());
    return url.size()==4 and
            url[0] == "api" and
            url[1] == "v1" and
            url[2] == "maps/" and
            game.FindMap(model::Map::Id(std::string(url[3]))) != nullptr;
}

template <typename Body, typename Allocator>
StringResponse MakeGetMapByIdResponse(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game){
    http::response<http::string_body> response(http::status::ok, req.version());
    auto id = SplitUrl(req.target())[3];
    response.set(http::field::content_type, "application/json");
    response.body() = jsonConverter::ConvertMapToJson(*game.FindMap(model::Map::Id(std::string(id))));
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
};

}