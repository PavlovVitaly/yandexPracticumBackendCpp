#pragma once

#include <string>

namespace prog_opt {

struct Args {
    size_t tick_period{0};
    std::string config_file;
    std::string www_root;
    bool randomize_spawn_points{false};
};

[[nodiscard]] Args ParseCommandLine(int argc, const char* const argv[]);

class ConfigFileNotSpecifiedException : public std::exception {
public:
    char const* what () {
        return "Config file have not been specified.";
    }
};

class StaticContentPathNotSpecifiedException : public std::exception {
public:
    char const* what () {
        return "Static content path is not specified.";
    }
};


}