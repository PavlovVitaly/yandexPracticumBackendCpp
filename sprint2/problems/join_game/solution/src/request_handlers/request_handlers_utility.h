#pragma once
#include <string_view>
#include <vector>

namespace rh_storage{

std::vector<std::string_view> SplitUrl(std::string_view str);

}