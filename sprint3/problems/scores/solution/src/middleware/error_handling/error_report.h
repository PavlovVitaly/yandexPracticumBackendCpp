#pragma once
#include <boost/beast/core.hpp>
#include <string_view>

namespace error_report {

namespace beast = boost::beast;

void ReportError(beast::error_code ec, std::string_view what);

}