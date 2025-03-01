#pragma once

#include <random>

#include "vectors.hpp"

class random {

public:

	static bool get_boolean (uint32_t chance) {
		return get_float() < (float)chance / 100.f;
	}

	static int get_int() {
		return uniform_int_distribution(random_engine);
	}

	static int get_int (int min, int max) {
		return min + (uniform_int_distribution(random_engine) % (max - min + 1));
	}

	// returns 0.f to 1.f float
	static float get_float() {
		return (float)uniform_int_distribution(random_engine) / (float)std::numeric_limits<uint32_t>::max();
	}

	static float get_full_range_float() {
		return uniform_real_distribution(random_engine);
	}

	static vec3_t get_vec3() {
		return vec3_t(get_float(), get_float(), get_float());
	}

	static vec3_t get_vec3 (float min, float max) {
		return vec3_t(get_float() * (max - min) + min, get_float() * (max - min) + min, get_float() * (max - min) + min);
	}

private:
	static std::default_random_engine random_engine;
	static std::uniform_int_distribution<std::mt19937::result_type> uniform_int_distribution;
	static std::uniform_real_distribution<float> uniform_real_distribution;
};

