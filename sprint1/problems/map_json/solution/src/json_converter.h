#pragma once
#include "model.h"
#include <string>


namespace jsonConverter{

std::string ConvertMapListToJson(const model::Game& game);
std::string ConvertMapToJson(const model::Map& map);
std::string CreateMapNotFoundResponse();
std::string CreateBadRequestResponse();
std::string CreatePageNotFoundResponse();

}