#pragma once
#include <string_view>
#include <vector>

namespace rh_storage{

std::vector<std::string_view> SplitUrl(std::string_view str);
std::string GetTokenString(std::string_view bearer_string);
bool IsEqualUrls(const std::string& server_url, const std::string_view request_url);

}