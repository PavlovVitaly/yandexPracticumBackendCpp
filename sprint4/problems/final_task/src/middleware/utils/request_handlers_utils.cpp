#include "request_handlers_utils.h"

#include <string>

namespace rh_storage{

const std::string BEARER = "Bearer";
const size_t TOKEN_SIZE = 32;
const size_t AUTHORIZATION_NUMBER_PARTS = 2;
const size_t BEARER_INDEX = 0;
const size_t TOKEN_INDEX = 1;

std::vector<std::string_view> SplitUrl(std::string_view str) {
    std::vector<std::string_view> result;
    std::string delim = "/";
    if(str.empty() || str == delim) return result;
    auto start = 1U; // Ignore first slash
    auto end = str.find(delim, start);
    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delim.length();
        end = str.find(delim, start);
    }
    result.push_back(str.substr(start, end));
    return result;
};

std::string GetTokenString(std::string_view bearer_string) {
    std::string token;
    std::vector<std::string_view> splitted;
    std::string delim = " ";
    if(bearer_string.empty() or bearer_string == delim) {
        return token;
    }
    auto start = 0U;
    auto end = bearer_string.find(delim, start);
    while (end != std::string::npos) {
        splitted.push_back(bearer_string.substr(start, end - start));
        start = end + delim.length();
        end = bearer_string.find(delim, start);
    }
    splitted.push_back(bearer_string.substr(start, end));
    if(splitted.size() != AUTHORIZATION_NUMBER_PARTS ||
    splitted[BEARER_INDEX] != BEARER ||
    splitted[TOKEN_INDEX].size() != TOKEN_SIZE) {
        return token;
    }
    return std::string(splitted[TOKEN_INDEX]);
};

bool IsEqualUrls(const std::string& server_url, const std::string_view request_url){
    return request_url == server_url || request_url == server_url + "/";
};

}