#pragma once

#include <string>
#include <chrono>

namespace saving {

struct SavingSettings {
    std::string state_file_path;
    std::chrono::milliseconds period;
};

}