#pragma once
#include <unordered_map>
#include <boost/beast/http.hpp>

namespace rh_storage{

namespace beast = boost::beast;
namespace http = beast::http;


template<typename Activator, typename Handler>
class RequestHandlerNode {
public:
    RequestHandlerNode(Activator activator, std::unordered_map<http::verb, Handler> handlers):
        activator_(std::move(activator)), handlers_(std::move(handlers)) {};

    RequestHandlerNode(const RequestHandlerNode& other) = default;
    RequestHandlerNode(RequestHandlerNode&& other) = default;
    RequestHandlerNode& operator = (const RequestHandlerNode& other) = default;
    RequestHandlerNode& operator = (RequestHandlerNode&& other) = default;
    virtual ~RequestHandlerNode() = default;

    template<typename Request>
    Handler& GetHandler(const Request& req, Handler& fault_handler) {
        http::verb method = req.method();
        if(handlers_.contains(method)) {
            return handlers_[method];
        } else {
            return fault_handler;
        }
    };

    Activator& GetActivator() {
      return activator_;
    };

private:
    Activator activator_;
    std::unordered_map<http::verb, Handler> handlers_;
};

}