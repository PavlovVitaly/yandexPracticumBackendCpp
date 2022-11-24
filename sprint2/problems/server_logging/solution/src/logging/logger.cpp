#include "logger.h"

namespace logware {

void InitLogger() {
    boost::log::add_console_log( 
        std::clog,
        boost::log::keywords::format = "%Message%"
    );
};

void MyFormatter(logging::record_view const& rec, logging::formatting_ostream& strm) {
    // Выводим само сообщение.
    strm << rec[expr::smessage];
} 

}