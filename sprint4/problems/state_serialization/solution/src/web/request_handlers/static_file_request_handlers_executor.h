#pragma once
#include "static_file_request_handlers_storage.h"
#include "request_handler_node.h"

#include <functional>
#include <vector>

namespace rh_storage{

template<typename Request, typename Send>
class StaticFileRequestHandlerExecutor{
    using ActivatorType = bool(*)(const Request&, const std::filesystem::path& );
    using HandlerType = void(*)(const Request&, const std::filesystem::path&, Send&&);
public:
    // убираем конструктор копирования
    StaticFileRequestHandlerExecutor(const StaticFileRequestHandlerExecutor&) = delete;
    StaticFileRequestHandlerExecutor& operator=(const StaticFileRequestHandlerExecutor&) = delete;
    StaticFileRequestHandlerExecutor(StaticFileRequestHandlerExecutor&&) = delete;
    StaticFileRequestHandlerExecutor& operator=(StaticFileRequestHandlerExecutor&&) = delete;

    // получение ссылки на единственный объект
    static StaticFileRequestHandlerExecutor& GetInstance() {
        static StaticFileRequestHandlerExecutor obj;
        return obj;
    };

    bool Execute(const Request& req, const std::filesystem::path& static_content_root, Send&& send) {
        for(auto item : rh_storage_) {
            if(item.GetActivator()(req, static_content_root)){
                item.GetHandler(req.method())(req, static_content_root, std::move(send));
                return true;
            }
        }
        return false;
    };

private:
    std::vector< RequestHandlerNode<ActivatorType, HandlerType> > rh_storage_ = {
        RequestHandlerNode<ActivatorType, HandlerType>(StaticContentFileNotFoundActivator,
                                                        {{http::verb::get, StaticContentFileNotFoundHandler}},
                                                        StaticContentFileNotFoundHandler),
        RequestHandlerNode<ActivatorType, HandlerType>(LeaveStaticContentRootDirActivator,
                                                        {{http::verb::get, LeaveStaticContentRootDirHandler}},
                                                        StaticContentFileNotFoundHandler),
        RequestHandlerNode<ActivatorType, HandlerType>(GetStaticContentFileActivator,
                                                        {{http::verb::get, GetStaticContentFileHandler}},
                                                        StaticContentFileNotFoundHandler)
    };

    StaticFileRequestHandlerExecutor() = default;
};

}