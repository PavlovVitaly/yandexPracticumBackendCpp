#include "responce_storage.h"

namespace responceStorage{

std::vector<std::string_view> SplitUrl(std::string_view str){
    std::vector<std::string_view> result;
    std::string delim = "/";
    if(str.empty() or str == delim) return result;
    auto start = 1U; // Ignore first slash
    auto end = str.find(delim, start);
    while (end != std::string::npos)
    {
        result.push_back(str.substr(start, end - start));
        start = end + delim.length();
        end = str.find(delim, start);
    }
    result.push_back(str.substr(start, end));
    return result;
};

}