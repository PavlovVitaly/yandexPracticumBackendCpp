#pragma once

#include <filesystem>

#include "game.h"

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path);

class OpenConfigFileOfModelException : public std::exception {
public:
    char const* what () {
        return "Can't open file with json configuration of model.";
    }
};

}  // namespace json_loader
