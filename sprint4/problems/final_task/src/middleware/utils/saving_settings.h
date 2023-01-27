#pragma once

#include <string>
#include <chrono>
#include <optional>

namespace saving {

struct SavingSettings {
    std::optional<std::string> state_file_path;
    std::optional<std::chrono::milliseconds> period;
};

}