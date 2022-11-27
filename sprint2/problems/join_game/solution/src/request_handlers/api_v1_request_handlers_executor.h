#pragma once
#include "api_v1_request_handlers_storage.h"
#include "request_handler_node.h"

#include <functional>
#include <vector>

namespace rh_storage{

template<typename Request, typename Send>
class ApiV1RequestHandlerExecutor{
    using ActivatorType = bool(*)(const Request&, const app::Application&);
    using HandlerType = void(*)(const Request&, const app::Application&, Send&&);
public:
    // убираем конструктор копирования
    ApiV1RequestHandlerExecutor(const ApiV1RequestHandlerExecutor&) = delete;
    ApiV1RequestHandlerExecutor& operator=(const ApiV1RequestHandlerExecutor&) = delete;
    ApiV1RequestHandlerExecutor(ApiV1RequestHandlerExecutor&&) = delete;
    ApiV1RequestHandlerExecutor& operator=(ApiV1RequestHandlerExecutor&&) = delete;

    // получение ссылки на единственный объект
    static ApiV1RequestHandlerExecutor& GetInstance() {
        static ApiV1RequestHandlerExecutor obj;
        return obj;
    };

    bool Execute(const Request& req, const app::Application& application, Send&& send) {
        for(auto item : rh_storage_) {
            if(item.GetActivator()(req, application)){
                item.GetHandler(req.method())(req, application, std::move(send));
                return true;
            }
        }
        return false;
    };

private:
    
    std::vector< RequestHandlerNode<ActivatorType, HandlerType> > rh_storage_ = {
        RequestHandlerNode<ActivatorType, HandlerType>(BadRequestActivator,
                                                        {{http::verb::get, BadRequestHandler}},
                                                        BadRequestHandler),
        RequestHandlerNode<ActivatorType, HandlerType>(GetMapListActivator,
                                                        {{http::verb::get, GetMapListHandler}},
                                                        BadRequestHandler),
        RequestHandlerNode<ActivatorType, HandlerType>(MapNotFoundActivator,
                                                        {{http::verb::get, MapNotFoundHandler}},
                                                        BadRequestHandler),
        RequestHandlerNode<ActivatorType, HandlerType>(GetMapByIdActivator,
                                                        {{http::verb::get, GetMapByIdHandler}},
                                                        BadRequestHandler)
    };

    ApiV1RequestHandlerExecutor() = default;
};

}