#pragma once
#include "model.h"
#include "json_converter.h"
#include <boost/beast/http.hpp>
#include <unordered_map>
#include <functional>
#include <memory>

namespace rcChain{

namespace beast = boost::beast;
namespace http = beast::http;
using StringResponse = http::response<http::string_body>;


template <typename Body, typename Allocator>
class ResponseCommandChain{
    using HttpRequest = http::request<Body, http::basic_fields<Allocator>>;
    using ResponseFunctionActivatorType = std::function<bool(const HttpRequest& req, const model::Game& game)>; 
    using ResponseFunctionType = std::function<StringResponse(const HttpRequest& req, const model::Game& game)>;
    using ResponseCommand = std::pair<ResponseFunctionActivatorType, std::unordered_map<http::verb, ResponseFunctionType>>;

public:

    static std::shared_ptr<ResponseCommandChain> CreateResponseCommandChain(
            std::shared_ptr<ResponseFunctionType> executeDefault,
            std::vector<ResponseCommand> commands){
        if(commands.empty()) return std::make_shared<ResponseCommandChain>();
        std::shared_ptr<ResponseCommandChain> tail = std::make_shared<ResponseCommandChain>();
        std::shared_ptr<ResponseCommandChain> head = std::make_shared<ResponseCommandChain>(commands[0], executeDefault);
        head->SetNext(tail);
        for(size_t i = 1; i < commands.size(); ++i){
            std::shared_ptr<ResponseCommandChain> tail = std::make_shared<ResponseCommandChain>(commands[i], executeDefault);
            tail->SetNext(std::make_shared<ResponseCommandChain>());
            tail = tail->next_;
        }
        return head;
    }

    ResponseCommandChain(const ResponseCommandChain& other) = delete;
    ResponseCommandChain(ResponseCommandChain&& other) = delete;
    ResponseCommandChain& operator = (const ResponseCommandChain& other) = delete;
    ResponseCommandChain& operator = (ResponseCommandChain&& other) = delete;
    ~ResponseCommandChain() = default;
    
    void SetNext(std::shared_ptr<ResponseCommandChain> next){
        next_ = next;
    }

    StringResponse Execute(const HttpRequest& req, const model::Game& game){
        if(!command_->first(req, game) and next_) return next_->Execute(req, game);
        if(!command_->second.contains(req.method()) or !next_) executeDefault(req, game);
        return command_->second[req.method()](req, game);
    };

private:
    ResponseCommand command_;
    std::shared_ptr<ResponseFunctionType> executeDefault_;
    std::shared_ptr<ResponseCommandChain> next_;

    ResponseCommandChain() = default;
    ResponseCommandChain(std::shared_ptr<ResponseCommand> command,
                        std::shared_ptr<ResponseFunctionType> executeDefault):
                        command_(std::move(command)), executeDefault_(executeDefault){};
};

}