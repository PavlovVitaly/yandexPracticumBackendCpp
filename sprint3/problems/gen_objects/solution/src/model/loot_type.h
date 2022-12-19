#pragma once
#include <string>

namespace model {

struct LootType {
    std::string name{""};
    std::string file{""};
    std::string type{""};
    int rotation{0};
    std::string color{""};
    double scale{0.0};
};


}