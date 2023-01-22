#pragma once
#include <string_view>
#include <vector>
#include <sstream>
#include <boost/url/params_view.hpp>

namespace rh_storage{

std::vector<std::string_view> SplitUrl(std::string_view str);
std::string GetTokenString(std::string_view bearer_string);
bool IsEqualUrls(const std::string& server_url, const std::string_view request_url);

template<typename T>
T GetValueFromUrlParameter(const boost::urls::params_view& params, const std::string& key) {
    std::stringstream ss;
    ss << (*params.find(key)).value;
    T res;
    ss >> res;
    return res;
};

}