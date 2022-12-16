#include "urldecode.h"

#include <charconv>
#include <stdexcept>
#include <string>
#include <sstream>
#include <regex>
#include <exception>

std::string UrlDecode(std::string_view str) {
    const std::regex hexRegex(R"([0-9a-fA-F]{2})");
    std::string res(str);
    std::replace(res.begin(), res.end(), '+', ' ');
    std::stringstream ss;
    int64_t start = 0;
    int64_t pos = res.find('%', start);
    while(pos!=std::string::npos) {
        std::stringstream sym;
        char sym_code = 0;
        if((pos - start) > 0) {
            ss << res.substr(start, (pos - start));
        }
        const std::string sym_str = res.substr(pos + 1, 2);
        sym << std::hex << sym_str;
        std::smatch match_res{};
        if(!std::regex_search(sym_str.begin(), sym_str.end(), match_res, hexRegex)) {
            throw std::invalid_argument("");
        }
        sym_code = stoul(sym.str(), nullptr, 16);;
        ss << sym_code;
        start = pos + 3;
        pos = res.find('%', start);
    }
    ss << res.substr(start, res.size());
    return ss.str();
}
