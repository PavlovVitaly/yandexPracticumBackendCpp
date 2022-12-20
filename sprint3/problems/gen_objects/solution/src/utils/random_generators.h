#pragma once
#include <random>

namespace utils {

double GenerateDoubleFromInterval(const double lower, const double upper);
int GenerateIntegerFromInterval(const int lower, const int upper);
size_t GenerateSizeTFromInterval(const size_t lower, const size_t upper);

}