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
void MyFormatter(logging::record_view const& rec, logging::formatting_ostream& strm);


template <class T>
std::string CreateLogMessage(std::string_view msg, T&& data) {
    return json::serialize(json::value_from(LogMessage<T>(std::forward<T>(data), msg)));
};

}