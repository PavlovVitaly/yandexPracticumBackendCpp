#pragma once
#include "http_server.h"
#include "model.h"
#include "json_converter.h"
namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;

using StringRequest = http::request<http::string_body>;
using StringResponse = http::response<http::string_body>;

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game)
        : game_{game} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        // Обработать запрос request и отправить ответ, используя send
        //send(MakeGetMapListResponse(req));
        send(MakeGetMapByIdResponse(req, "map1"));
    }

private:
    model::Game& game_;

    template <typename Body, typename Allocator>
    StringResponse MakeGetMapListResponse(const http::request<Body, http::basic_fields<Allocator>>& req) {
        StringResponse response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonConverter::ConvertMapListToJson(game_);
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        return response;
    }

    template <typename Body, typename Allocator>
    StringResponse MakeGetMapByIdResponse(const http::request<Body, http::basic_fields<Allocator>>& req,
                                            const std::string& id) {
        StringResponse response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonConverter::ConvertMapToJson(*game_.FindMap(model::Map::Id(id)));
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        return response;
    }

};

}  // namespace http_handler
