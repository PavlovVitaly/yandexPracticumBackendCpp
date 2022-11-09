#pragma once
#include "http_server.h"
#include "model.h"
#include "responce_storage.h"
namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
using StringResponse = http::response<http::string_body>;

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game)
        : game_{game} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    /*
    # Обработчик отвратительный, есть идея как сделать его более гибким, но из-за шаблонов другая реализация пока не взлетела (надо обсудить).
    */
    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {// 
        // Обработать запрос request и отправить ответ, используя send
        if(responceStorage::UseBadRequestActivator(req, game_)){
            send(responceStorage::MakeBadRequestResponse(req, game_));
        }else if(responceStorage::UseGetMapListActivator(req, game_)){
            if(req.method() == http::verb::get) send(responceStorage::MakeGetMapListResponse(req, game_));
            else send(responceStorage::MakeBadRequestResponse(req, game_));
        }else if(responceStorage::UseMapNotFoundActivator(req, game_)){
            if(req.method() == http::verb::get) send(responceStorage::MakeMapNotFoundResponse(req, game_));
            else send(responceStorage::MakeBadRequestResponse(req, game_));
        }else if(responceStorage::UseGetMapByIdActivator(req, game_)){
            if(req.method() == http::verb::get) send(responceStorage::MakeGetMapByIdResponse(req, game_));
            else send(responceStorage::MakeBadRequestResponse(req, game_));
        }else{
            send(responceStorage::MakePageNotFoundResponse(req, game_));
        }
    }

private:
    model::Game& game_;
    
};

}  // namespace http_handler
