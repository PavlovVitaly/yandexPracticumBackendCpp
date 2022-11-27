#pragma once
#include "game.h"
#include <string>


namespace json_converter{

std::string ConvertMapListToJson(const model::Game::Maps& maps);
std::string ConvertMapToJson(const model::Map& map);
std::string CreateMapNotFoundResponse();
std::string CreateBadRequestResponse();
std::string CreatePageNotFoundResponse();

}