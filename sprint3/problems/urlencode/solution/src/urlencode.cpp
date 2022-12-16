#include "urlencode.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <unordered_set>

const std::unordered_set<char> RESERVED_SYMBOLS = {
    '!', '#', '$', '&', '\'', '(', ')', '*', ',', '/', ':', ';', '=', '?', '@', '[', ']'
};

std::string UrlEncode(std::string_view str) {
    std::string res(str);
    std::replace(res.begin(), res.end(), ' ', '+');
    std::stringstream ss;
    for(auto letter : res){
        uint16_t sym = letter;
        if(letter != '+' && (sym < 32 || sym > 128 || RESERVED_SYMBOLS.contains(letter))){
            ss << "%" << std::hex << sym;
        } else {
            ss << letter;
        }
    }
    auto a1 = ss.str(); 
    return ss.str();
}
