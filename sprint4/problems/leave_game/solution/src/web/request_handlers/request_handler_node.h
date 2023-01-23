#pragma once
#include <unordered_map>
#include <boost/beast/http.hpp>
#include <vector>

namespace rh_storage{

namespace beast = boost::beast;
namespace http = beast::http;


template<typename Activator, typename Handler>
class RequestHandlerNode {
public:
    RequestHandlerNode(Activator activator,
                        std::unordered_map<http::verb, Handler> handlers,
                        Handler not_found_handler,
                        std::vector<Handler>&& emerge_handlers = std::vector<Handler>()) :
        activator_(std::move(activator)),
        handlers_(std::move(handlers)),
        not_found_handler_(not_found_handler),
        emerge_handlers_(std::move(emerge_handlers)) {};

    RequestHandlerNode(const RequestHandlerNode& other) = default;
    RequestHandlerNode(RequestHandlerNode&& other) = default;
    RequestHandlerNode& operator = (const RequestHandlerNode& other) = default;
    RequestHandlerNode& operator = (RequestHandlerNode&& other) = default;
    virtual ~RequestHandlerNode() = default;

    Handler& GetHandler(http::verb method) {
        if(handlers_.contains(method)) {
            return handlers_[method];
        } else {
            return not_found_handler_;
        }
    };

    std::optional<Handler> GetEmergeHandlerByIndex(size_t index) {
        if(index < emerge_handlers_.size()) {
            return emerge_handlers_[index];
        }
        return std::nullopt;
    };

    Activator& GetActivator() {
      return activator_;
    };

private:
    Activator activator_;
    std::unordered_map<http::verb, Handler> handlers_;
    Handler not_found_handler_;
    std::vector<Handler> emerge_handlers_;
};

}