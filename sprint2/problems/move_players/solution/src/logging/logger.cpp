#include "logger.h"

namespace logware {

void StringFormatter(logging::record_view const& rec, logging::formatting_ostream& strm) {
    // Выводим само сообщение.
    strm << rec[expr::smessage];
} 

void InitLogger() {
    boost::log::add_console_log( 
        std::cout,
        keywords::auto_flush = true,
        boost::log::keywords::format = &StringFormatter//"%Message%"
    );
};

}