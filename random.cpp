#include "random.hpp"

std::default_random_engine random::random_engine;
std::uniform_int_distribution<std::mt19937::result_type> random::uniform_int_distribution;
std::uniform_real_distribution<float> random::uniform_real_distribution(-1.f, 1.f);