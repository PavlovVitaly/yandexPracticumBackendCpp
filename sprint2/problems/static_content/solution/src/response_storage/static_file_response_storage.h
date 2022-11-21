#pragma once
#include <vector>
#include <boost/beast/http.hpp>
#include <unordered_map>
#include <string>
#include <iostream>

#include "filesystem_utils.h" 

namespace responce_storage{

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


template <typename Body, typename Allocator>
bool UseStaticContentFileNotFoundActivator(
        const http::request<Body, http::basic_fields<Allocator>>& req,
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

template <typename Body, typename Allocator>
StringResponse MakeStaticContentFileNotFoundResponse(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const fs::path& static_content_root) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "text/plain");
    response.body() = "Static content file not found.";
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
};


template <typename Body, typename Allocator>
bool UseLeaveStaticContentRootDirActivator(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const fs::path& static_content_root) {
    fs::path static_content{static_content_root};
    std::string_view pathStr = req.target().substr(1, req.target().size() - 1);
    fs::path rel_path{pathStr};
    static_content = fs::weakly_canonical(static_content / rel_path);
    return !fs_utils::IsSubPath(static_content, static_content_root);
};

template <typename Body, typename Allocator>
StringResponse MakeLeaveStaticContentRootDirResponse(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const fs::path& static_content_root) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "text/plain");
    response.body() = "Try to leave static content root directory.";
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
};


template <typename Body, typename Allocator>
bool UseGetStaticContentFileActivator(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const fs::path& static_content_root) {
    return true;
};

template <typename Body, typename Allocator>
http::response<http::file_body> MakeGetStaticContentFileResponse(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const fs::path& static_content_root) {
    http::response<http::file_body> res;
    res.version(11);  // HTTP/1.1
    res.result(http::status::ok);

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
        res.insert(http::field::content_type, EXTENSION_FILE_TO_CONTENT_TYPE.at(static_content.extension()));
    } else {
        res.insert(http::field::content_type, "application/octet-stream");
    }
    
    http::file_body::value_type file;

    if (sys::error_code ec; file.open(static_content.c_str(), beast::file_mode::read, ec), ec) {
        std::cout << "Failed to open static content file "sv << std::endl;  // todo: error handler
    } else {
        res.body() = std::move(file);
    }

    // Метод prepare_payload заполняет заголовки Content-Length и Transfer-Encoding
    // в зависимости от свойств тела сообщения
    res.prepare_payload();
    return res;
};

}