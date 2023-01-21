#pragma once

#include <string>

namespace model {

struct DogRecord {
    std::string name{};
    size_t score{0};
    int64_t play_time{0};
};

}