#pragma once

#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <algorithm>

using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {
    auto GetTime() const {
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        return std::put_time(std::localtime(&t_c), "%F %T");
    }

    // Для имени файла возьмите дату с форматом "%Y_%m_%d"
    std::string GetFileTimeStamp() const{
        std::stringstream ss;
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        ss << std::put_time(std::localtime(&t_c), "%F");
        std::string str = ss.str();
        std::replace(str.begin(), str.end(), '-', '_');
        return str;
    };

    Logger(){
        std::string log_file = LOG_DIRECTORY + "sample_log_" + GetFileTimeStamp() + LOG_EXTENSION;
        //std::string log_file = "./sample_log_" + GetFileTimeStamp();
        log_file_.open(log_file, std::ios::app);
    };
    Logger(const Logger&) = delete;

public:
    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    // Выведите в поток все аргументы.
    template<class... Ts>
    void Log(const Ts&... args){
        std::shared_lock guard(mtx_);
        log_file_ << GetTimeStamp() << ": "sv;
        ((log_file_ << args), ...);
        log_file_ << std::endl;
    };
    
    // Установите manual_ts_. Учтите, что эта операция может выполняться
    // параллельно с выводом в поток, вам нужно предусмотреть 
    // синхронизацию.
    void SetTimestamp(std::chrono::system_clock::time_point ts){
        std::lock_guard guard(mtx_);
        manual_ts_ = ts;
        log_file_.close();
        std::string log_file = LOG_DIRECTORY + "sample_log_" + GetFileTimeStamp() + LOG_EXTENSION;
        log_file_.open(log_file, std::ios::app);

    };

    ~Logger(){
        log_file_.close();
    }

private:
    const std::string LOG_DIRECTORY{"/var/log/"};
    const std::string LOG_EXTENSION{".log"};
    std::optional<std::chrono::system_clock::time_point> manual_ts_;
    mutable std::shared_mutex mtx_;
    std::ofstream log_file_;
};
