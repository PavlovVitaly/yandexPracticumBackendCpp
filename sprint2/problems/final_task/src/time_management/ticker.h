#pragma once

#include <memory>
#include <chrono>
#include <functional>
#include <boost/asio/strand.hpp>
#include <boost/asio.hpp>

namespace time_m {

namespace net = boost::asio;
namespace sys = boost::system;

class Ticker : public std::enable_shared_from_this<Ticker> {
public:
    using Strand = net::strand<net::io_context::executor_type>;
    using Handler = std::function<void(const std::chrono::milliseconds& delta)>;

    Ticker(std::shared_ptr<Strand> strand, const std::chrono::milliseconds& period, Handler handler):
        strand_{strand},
        timer_{*strand_},
        period_{period},
        handler_{handler}{
    };
    ~Ticker() = default;

    void Start();
private:
    std::shared_ptr<Strand> strand_;;
    net::steady_timer timer_;
    std::chrono::milliseconds period_;
    Handler handler_;
    std::chrono::time_point<std::chrono::steady_clock> last_tick_;

    void ScheduleTick();
    void OnTick(sys::error_code ec);
}; 

}