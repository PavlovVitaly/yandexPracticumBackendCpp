#pragma once
#include <boost/log/trivial.hpp>     // для BOOST_LOG_TRIVIAL
#include <boost/log/core.hpp>        // для logging::core
#include <boost/log/utility/setup/console.hpp>

#include "logging_data_storage.h"

namespace logware {

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace expr = logging::expressions;
namespace json = boost::json;
using namespace std::literals;

void InitLogger();

template <class T>
std::string CreateLogMessage(std::string_view msg, T&& data) {
    return json::serialize(json::value_from(LogMessage<T>(msg, std::forward<T>(data))));
};

}