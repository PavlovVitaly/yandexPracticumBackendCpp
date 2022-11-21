#pragma once
#include "http_server.h"
#include "model.h"
#include "api_v1_responce_storage.h"
#include "static_file_response_storage.h"
#include <filesystem>
namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
using StringResponse = http::response<http::string_body>;
namespace fs = std::filesystem;

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game, fs::path static_content_root_path)
        : game_{game}, static_content_root_path_{static_content_root_path} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    /*
    # Обработчик отвратительный, есть идея как сделать его более гибким, но из-за шаблонов другая реализация пока не взлетела (надо обсудить).
    */
    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {// 
        // Обработать запрос request и отправить ответ, используя send
        if(responce_storage::UseBadRequestActivator(req, game_)) {
            send(responce_storage::MakeBadRequestResponse(req, game_));
        } else if(responce_storage::UseGetMapListActivator(req, game_)) {
            if(req.method() == http::verb::get) {
                send(responce_storage::MakeGetMapListResponse(req, game_));
            } else {
                send(responce_storage::MakeBadRequestResponse(req, game_));
            }
        } else if(responce_storage::UseMapNotFoundActivator(req, game_)) {
            if(req.method() == http::verb::get) {
                send(responce_storage::MakeMapNotFoundResponse(req, game_));
            } else {
                send(responce_storage::MakeBadRequestResponse(req, game_));
            }
        } else if(responce_storage::UseGetMapByIdActivator(req, game_)) {
            if(req.method() == http::verb::get) {
                send(responce_storage::MakeGetMapByIdResponse(req, game_));
            } else {
                send(responce_storage::MakeBadRequestResponse(req, game_));
            }
        } else if(responce_storage::UseStaticContentFileNotFoundActivator(req, static_content_root_path_)) {       // static content
            if(req.method() == http::verb::get || req.method() == http::verb::head) {
                send(responce_storage::MakeStaticContentFileNotFoundResponse(req, static_content_root_path_));
            } else {
                send(responce_storage::MakeBadRequestResponse(req, game_));
            }
        } else if(responce_storage::UseLeaveStaticContentRootDirActivator(req, static_content_root_path_)) {
            if(req.method() == http::verb::get || req.method() == http::verb::head) {
                send(responce_storage::MakeLeaveStaticContentRootDirResponse(req, static_content_root_path_));
            } else {
                send(responce_storage::MakeBadRequestResponse(req, game_));
            }
        }else if(responce_storage::UseGetStaticContentFileActivator(req, static_content_root_path_)) {
            if(req.method() == http::verb::get || req.method() == http::verb::head) {
                send(responce_storage::MakeGetStaticContentFileResponse(req, static_content_root_path_));
            } else {
                send(responce_storage::MakeBadRequestResponse(req, game_));
            }
        } else {
            send(responce_storage::MakePageNotFoundResponse(req, game_));
        }
    }

private:
    model::Game& game_;
    fs::path static_content_root_path_;
    
};

}  // namespace http_handler
