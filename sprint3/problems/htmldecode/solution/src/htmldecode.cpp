#include "htmldecode.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

const std::unordered_map<std::string, std::string> CODE_TABLE = {
    {"&lt", "<"},
    {"&gt", ">"},
    {"&amp", "&"},
    {"&apos", "\'"},
    {"&quot", "\""}
};

std::string HtmlDecode(std::string_view str) {
    std::string res(str);
    std::replace(res.begin(), res.end(), '+', ' ');
    std::stringstream ss;
    int64_t start = 0;
    int64_t pos = res.find('&', start);
    std::unordered_set<size_t> sizes = GetKeysSizes(CODE_TABLE);
    while(pos!=std::string::npos) {
        if((pos - start) > 0) {
            ss << res.substr(start, (pos - start));
        }
        size_t offset = 0;
        for(auto s : sizes) {
            std::string mnemonic = res.substr(pos, s);
            std::string temp_mnemonic = mnemonic;
            ToLowerCase(mnemonic);
            if(CODE_TABLE.contains(mnemonic)) {
                if(temp_mnemonic != mnemonic) {
                    ToUpperCase(temp_mnemonic);
                    if(temp_mnemonic != res.substr(pos, s)) {
                        break;
                    }
                }
                ss << CODE_TABLE.at(mnemonic);
                offset = res[pos + s] == ';' ? s+1 : s;
                break;
            }
        }
        if(!offset) {
            ss << res[pos];
            start = pos + 1;
        } else {
            start = pos + offset;
        }
        pos = res.find('&', start);
    }
    ss << res.substr(start, res.size());
    return ss.str();
}

std::unordered_set<size_t> GetKeysSizes(const std::unordered_map<std::string, std::string>& table) {
    std::unordered_set<size_t> sizes;
    for(auto [key, value] : CODE_TABLE) {
        sizes.insert(key.size());
    }
    return sizes;
}

void ToLowerCase(std::string& str){
    std::transform(str.begin(),
                    str.end(),
                    str.begin(),
                    [](unsigned char c){
                        return std::tolower(c);
                    });
}

void ToUpperCase(std::string& str){
    std::transform(str.begin(),
                    str.end(),
                    str.begin(),
                    [](unsigned char c){
                        return std::toupper(c);
                    });
}