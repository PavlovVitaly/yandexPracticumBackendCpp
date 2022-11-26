#pragma once
#include "http_server.h"
#include "model.h"
#include "api_v1_request_handlers_executor.h"
#include "static_file_request_handlers_executor.h"

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
        if(rh_storage::ApiV1RequestHandlerExecutor<http::request<Body, http::basic_fields<Allocator>>, Send>
            ::GetInstance()
            .Execute(req, game_, std::move(send))) {
            return;
        } else if(rh_storage::StaticFileRequestHandlerExecutor<http::request<Body, http::basic_fields<Allocator>>, Send>
            ::GetInstance()
            .Execute(req, static_content_root_path_, std::move(send))) {
            return;
        };
        rh_storage::PageNotFoundHandler(req, game_, send);
    }

private:
    model::Game& game_;
    fs::path static_content_root_path_;
    
};

}  // namespace http_handler
