#pragma once
#include "filesystem_utils.h" 
#include "logger.h"

#include <vector>
#include <boost/beast/http.hpp>
#include <unordered_map>
#include <string>

namespace rh_storage{

namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;
namespace fs = std::filesystem;
using StringResponse = http::response<http::string_body>;
using namespace std::literals;

const std::unordered_map<std::string, std::string> EXTENSION_FILE_TO_CONTENT_TYPE = {
    {".htm", "text/html"},
    {".html", "text/html"},
    {".css", "text/css"},
    {".txt", "text/plain"},
    {".js", "text/javascript"},
    {".json", "application/json"},
    {".xml", "application/xml"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpe", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif", "image/gif"},
    {".bmp", "image/bmp"},
    {".ico", "image/vnd.microsoft.icon"},
    {".tiff", "image/tiff"},
    {".tif", "image/tiff"},
    {".svg", "image/svg+xml"},
    {".svgz", "image/svg+xml"},
    {".mp3", "audio/mpeg"}
};

const std::string INDEX_FILE_NAME{"index.html"};


template <typename Request>
bool StaticContentFileNotFoundActivator(
        const Request& req,
        const fs::path& static_content_root) {
    fs::path static_content{static_content_root};
    if(req.target().empty() || req.target() == "/") {
        fs::path rel_path{INDEX_FILE_NAME};
        static_content = fs::weakly_canonical(static_content / rel_path);
    } else {
        std::string_view pathStr = req.target().substr(1, req.target().size() - 1);
        fs::path rel_path{pathStr};
        static_content = fs::weakly_canonical(static_content / rel_path);
        if(fs::is_directory(static_content)) {
            fs::path rel_path{INDEX_FILE_NAME};
            static_content = fs::weakly_canonical(static_content / rel_path);
        }
    }
    return !fs::exists(static_content);
};

template <typename Request, typename Send>
void StaticContentFileNotFoundHandler(
        const Request& req,
        const fs::path& static_content_root,
        Send&& send) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "text/plain");
    response.body() = "Static content file not found.";
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
};


template <typename Request>
bool LeaveStaticContentRootDirActivator(
        const Request& req,
        const fs::path& static_content_root) {
    fs::path static_content{static_content_root};
    std::string_view pathStr = req.target().substr(1, req.target().size() - 1);
    fs::path rel_path{pathStr};
    static_content = fs::weakly_canonical(static_content / rel_path);
    return !fs_utils::IsSubPath(static_content, static_content_root);
};

template <typename Request, typename Send>
void LeaveStaticContentRootDirHandler(
        const Request& req,
        const fs::path& static_content_root,
        Send&& send) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "text/plain");
    response.body() = "Try to leave static content root directory.";
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    send(response);
};


template <typename Request>
bool GetStaticContentFileActivator(
        const Request& req,
        const fs::path& static_content_root) {
    return true;
};

template <typename Request, typename Send>
void GetStaticContentFileHandler(
        const Request& req,
        const fs::path& static_content_root,
        Send&& send) {
    http::response<http::file_body> response(http::status::ok, req.version());

    fs::path static_content{static_content_root};
    if(req.target().empty() || req.target() == "/") {
        fs::path rel_path{INDEX_FILE_NAME};
        static_content = fs::weakly_canonical(static_content / rel_path);
    } else {
        std::string_view pathStr = req.target().substr(1, req.target().size() - 1);
        fs::path rel_path{pathStr};
        static_content = fs::weakly_canonical(static_content / rel_path);
    }
    if(EXTENSION_FILE_TO_CONTENT_TYPE.contains(static_content.extension())) {
        response.insert(http::field::content_type, EXTENSION_FILE_TO_CONTENT_TYPE.at(static_content.extension()));
    } else {
        response.insert(http::field::content_type, "application/octet-stream");
    }
    
    http::file_body::value_type file;

    if (sys::error_code ec; file.open(static_content.c_str(), beast::file_mode::read, ec), ec) {
        BOOST_LOG_TRIVIAL(error) << logware::CreateLogMessage("error"sv,
                                        logware::ExceptionLogData(0,
                                            "Failed to open static content file "sv,
                                            ec.what()));
    } else {
        response.body() = std::move(file);
    }

    // Метод prepare_payload заполняет заголовки Content-Length и Transfer-Encoding
    // в зависимости от свойств тела сообщения
    response.prepare_payload();
    send(response);
};

}