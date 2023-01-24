#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <boost/date_time.hpp>

namespace logware {

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
using HttpRequest = http::request<http::string_body>;
using namespace std::literals;


const std::string IP = "ip";
const std::string URL = "URI";
const std::string METHOD = "method";
const std::string RESPONSE_TIME = "response_time";
const std::string CODE = "code";
const std::string CONTENT_TYPE = "content_type";
const std::string PORT = "port";
const std::string ADDRESS = "address";
const std::string TEXT = "text";
const std::string WHERE = "where";
const std::string TIMESTAMP = "timestamp";
const std::string DATA = "data";
const std::string MESSAGE = "message";

struct RequestLogData {
    RequestLogData(std::string ip_addr, const HttpRequest& req):
            ip(ip_addr),
            url(req.target()),
            method(req.method_string()) {};

    std::string ip;
    std::string url;
    std::string method;
};

void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const RequestLogData& request);

template <typename Body, typename Fields>
struct ResponseLogData {
    ResponseLogData(std::string ip_addr, long res_time, const http::response<Body, Fields>& res):
            ip(ip_addr),
            response_time(res_time),
            code(res.result_int()),
            content_type(res[http::field::content_type]) {};

    std::string ip;
    long response_time;
    int code;
    std::string content_type;
};

template <typename Body, typename Fields>
void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ResponseLogData<Body, Fields>& response) {
    jv = {{IP, json::value_from(response.ip)},
            {RESPONSE_TIME, json::value_from(response.response_time)},
            {CODE, json::value_from(response.code)},
            {CONTENT_TYPE, json::value_from(response.content_type)}};
};

struct ServerAddressLogData {
    ServerAddressLogData(std::string addr, uint32_t prt): 
        address(addr), port(prt) {};

    std::string address;
    uint32_t port;
};

void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ServerAddressLogData& server_address);

struct ExceptionLogData {
    ExceptionLogData(int code, std::string_view text, std::string_view where):
        code(code), text(text), where(where) {};

    int code;
    std::string_view text;
    std::string_view where;
};

void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ExceptionLogData& exception);

struct ExitCodeLogData {
    int code;
};

void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, ExitCodeLogData const& exit_code);

template <class T>
struct LogMessage{
    LogMessage(std::string_view msg, T&& custom_data):
        message(msg), data(custom_data){
        timestamp = boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock::local_time());
    };
    
    std::string_view message;
    T data;
    std::string timestamp;
};

template <class T>
void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const LogMessage<T>& msg){
    jv = {{TIMESTAMP, json::value_from(msg.timestamp)},
            {DATA, json::value_from(msg.data)},
            {MESSAGE, json::value_from(msg.message)}};
};

}