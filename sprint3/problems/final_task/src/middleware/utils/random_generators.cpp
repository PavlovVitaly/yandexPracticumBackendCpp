#include "random_generators.h"
#include <random>

namespace utils {

double GenerateDoubleFromInterval(const double lower, const double upper) {
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> distr(lower, upper);
    return distr(eng);
};

int GenerateIntegerFromInterval(const int lower, const int upper) {
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_int_distribution<int> distr(lower,upper);
    return distr(eng);
};

size_t GenerateSizeTFromInterval(const size_t lower, const size_t upper) {
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_int_distribution<size_t> distr(lower,upper);
    return distr(eng);
};

}