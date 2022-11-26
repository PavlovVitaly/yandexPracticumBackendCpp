#pragma once
#include "api_v1_request_handlers_storage.h"
#include "request_handler_node.h"

#include <functional>
#include <vector>

namespace rh_storage{

template<typename Request, typename Send>
class ApiV1RequestHandlerExecutor{
    using ActivatorType = bool(*)(const Request&, const model::Game&);
    using HandlerType = void(*)(const Request&, const model::Game&, Send&&);
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

    bool Execute(const Request& req, const model::Game& game, Send&& send) {
        for(auto item : rh_storage_) {
            if(item.GetActivator()(req, game)){
                item.GetHandler(req, fault_handler_)(req, game, std::move(send));
                return true;
            }
        }
        return false;
    };

private:
    std::vector< RequestHandlerNode<ActivatorType, HandlerType> > rh_storage_ = {
        RequestHandlerNode<ActivatorType, HandlerType>(BadRequestActivator,
                                                        {{http::verb::get, BadRequestHandler}}),
        RequestHandlerNode<ActivatorType, HandlerType>(GetMapListActivator,
                                                        {{http::verb::get, GetMapListHandler}}),
        RequestHandlerNode<ActivatorType, HandlerType>(MapNotFoundActivator,
                                                        {{http::verb::get, MapNotFoundHandler}}),
        RequestHandlerNode<ActivatorType, HandlerType>(GetMapByIdActivator,
                                                        {{http::verb::get, GetMapByIdHandler}})
    };
    HandlerType fault_handler_ = BadRequestHandler;

    ApiV1RequestHandlerExecutor() = default;
};

}