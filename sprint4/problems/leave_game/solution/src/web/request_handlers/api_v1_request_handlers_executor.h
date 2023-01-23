#pragma once
#include "api_v1_request_handlers_storage.h"
#include "request_handler_node.h"

#include <functional>
#include <vector>
#include <optional>

namespace rh_storage{

namespace net = boost::asio;

template<typename Request, typename Send>
class ApiV1RequestHandlerExecutor{
    using ActivatorType = bool(*)(const Request&);
    using HandlerType = std::optional<size_t>(*)(const Request&, std::shared_ptr<app::Application>, Send&&);
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

    bool Execute(const Request& req, std::shared_ptr<app::Application> application, Send&& send) {
        for(auto item : rh_storage_) {
            if(item.GetActivator()(req)){
                    auto res = item.GetHandler(req.method())(req, application, std::forward<Send>(send));
                    while(res.has_value()){
                        res = item.GetEmergeHandlerByIndex(res.value())
                            .value()(req, application, std::forward<Send>(send));
                    }
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

        RequestHandlerNode<ActivatorType, HandlerType>(GetMapByIdActivator,
                                                        {{http::verb::get, GetMapByIdHandler},
                                                        {http::verb::head, GetMapByIdHandler}},
                                                        InvalidMethodHandler,
                                                        {MapNotFoundHandler}),
                                                        
        RequestHandlerNode<ActivatorType, HandlerType>(InvalidContentTypeActivator,
                                                        {{http::verb::post, InvalidContentTypeHandler}},
                                                        InvalidContentTypeHandler),

        RequestHandlerNode<ActivatorType, HandlerType>(JoinToGameInvalidJsonReqActivator,
                                                        {{http::verb::post, JoinToGameInvalidJsonReqHandler}},
                                                        OnlyPostMethodAllowedHandler),
        RequestHandlerNode<ActivatorType, HandlerType>(JoinToGameEmptyPlayerNameActivator,
                                                        {{http::verb::post, JoinToGameEmptyPlayerNameHandler}},
                                                        OnlyPostMethodAllowedHandler),
        RequestHandlerNode<ActivatorType, HandlerType>(JoinToGameActivator,
                                                        {{http::verb::post, JoinToGameHandler}},
                                                        OnlyPostMethodAllowedHandler,
                                                        {JoinToGameMapNotFoundHandler}),

        RequestHandlerNode<ActivatorType, HandlerType>(GetRecordsActivator,
                                                        {{http::verb::get, GetRecordsHandler}},
                                                        InvalidMethodHandler,
                                                        {BadRequestHandler}),

        RequestHandlerNode<ActivatorType, HandlerType>(EmptyAuthorizationActivator,
                                                        {{http::verb::get, EmptyAuthorizationHandler},
                                                        {http::verb::head, EmptyAuthorizationHandler}},
                                                        InvalidMethodHandler),

        RequestHandlerNode<ActivatorType, HandlerType>(GetPlayersListActivator,
                                                        {{http::verb::get, GetPlayersListHandler},
                                                        {http::verb::head, GetPlayersListHandler}},
                                                        InvalidMethodHandler,
                                                        {UnknownTokenHandler}),
        RequestHandlerNode<ActivatorType, HandlerType>(GetGameStateActivator,
                                                        {{http::verb::get, GetGameStateHandler},
                                                        {http::verb::head, GetGameStateHandler}},
                                                        InvalidMethodHandler,
                                                        {UnknownTokenHandler}),

        RequestHandlerNode<ActivatorType, HandlerType>(PlayerActionInvalidActionActivator,
                                                        {{http::verb::post, PlayerActionInvalidActionHandler}},
                                                        OnlyPostMethodAllowedHandler),
        RequestHandlerNode<ActivatorType, HandlerType>(PlayerActionActivator,
                                                        {{http::verb::post, PlayerActionHandler}},
                                                        InvalidMethodHandler,
                                                        {UnknownTokenHandler}),
        
        RequestHandlerNode<ActivatorType, HandlerType>(TimeTickInvalidMsgActivator,
                                                        {{http::verb::post, TimeTickInvalidMsgHandler}},
                                                        InvalidMethodHandler,
                                                        {InvalidEndpointHandler}),
        RequestHandlerNode<ActivatorType, HandlerType>(TimeTickActivator,
                                                        {{http::verb::post, TimeTickHandler}},
                                                        InvalidMethodHandler,
                                                        {InvalidEndpointHandler})
    };

    ApiV1RequestHandlerExecutor() = default;
};

}