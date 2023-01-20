#include "ticker.h"
#include "error_report.h"

namespace time_m {

using namespace std::literals;

void Ticker::Start() {
    last_tick_ = std::chrono::steady_clock::now();
    /* Выполнить SchedulTick внутри strand_ */
    ScheduleTick();
}

void Ticker::ScheduleTick() {
    /* выполнить OnTick через промежуток времени period_ */
    timer_.expires_after(period_);
    if(strand_) {
        timer_.async_wait(net::bind_executor(*strand_, [self = shared_from_this()](sys::error_code ec) {
            self->OnTick(ec);
        }));
    } else {
        timer_.async_wait([self = shared_from_this()](sys::error_code ec) {
            self->OnTick(ec);
        });
    }
}

void Ticker::OnTick(sys::error_code ec) {
    if (ec) {
        error_report::ReportError(ec, "Update game state timer tick"sv);
    }
    std::chrono::time_point<std::chrono::steady_clock> current_tick = std::chrono::steady_clock::now();
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_tick - last_tick_);
    handler_(duration);
    last_tick_ = current_tick;
    ScheduleTick();
}

}