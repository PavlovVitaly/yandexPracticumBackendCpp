#include "responce_storage.h"

namespace responceStorage{

std::vector<std::string_view> SplitUrl(std::string_view str){
    std::vector<std::string_view> result;
    std::string delim = "/";
    if(str.empty() or str == delim) return result;
    auto tmpStr = str.substr(1);
    auto start = 0U;
    auto end = tmpStr.find(delim);
    while (end != std::string::npos)
    {
        result.push_back(tmpStr.substr(start, end - start));
        start = end + delim.length();
        end = tmpStr.find(delim, start);
    }
    result.push_back(tmpStr.substr(start, end));
    return result;
};

}