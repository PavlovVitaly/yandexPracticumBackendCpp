#pragma once
#include "http_server.h"
#include "model.h"
//#include "responder.h"
#include "responce_storage.h"
//#include "response_command_chain.h"
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

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {// 
        // Обработать запрос request и отправить ответ, используя send
        if(responceStorage::MakeBadRequestResponseActivator(req, game_)){
            send(responceStorage::MakeBadRequestResponse(req, game_));
        }else if(responceStorage::MakeGetMapListResponseActivator(req, game_)){
            if(req.method() == http::verb::get) send(responceStorage::MakeGetMapListResponse(req, game_));
            else send(responceStorage::MakeBadRequestResponse(req, game_));
        }else if(responceStorage::MakeMapNotFoundResponseActivator(req, game_)){
            if(req.method() == http::verb::get) send(responceStorage::MakeMapNotFoundResponse(req, game_));
            else send(responceStorage::MakeBadRequestResponse(req, game_));
        }else if(responceStorage::MakeGetMapByIdResponseActivator(req, game_)){
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
