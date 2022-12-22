#include "error_report.h"
#include "logger.h"

namespace error_report {

using namespace std::literals;

void ReportError(beast::error_code ec, std::string_view where) {
    BOOST_LOG_TRIVIAL(error) << logware::CreateLogMessage("error"sv,
                                        logware::ExceptionLogData(0,
                                            ec.what(),
                                            where));
}

}