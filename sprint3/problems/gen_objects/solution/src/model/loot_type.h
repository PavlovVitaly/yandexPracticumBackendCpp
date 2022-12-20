#pragma once
#include <string>
#include <limits.h>

namespace model {

struct LootType {
    std::string name{""};
    std::string file{""};
    std::string type{""};
    int rotation{INT_MIN};
    std::string color{""};
    double scale{0.0};
};


}