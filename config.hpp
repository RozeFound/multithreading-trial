#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "vectors.hpp"

enum class cpy {
	assign, // make use of assign operator; somewhat fast
	function, // tediously copy over all properties; hella slow
	std_copy, // use std::copy; hella slow
	memcpy // make raw copy; it's a safety hazard but it's fast
};

namespace config {

	constexpr std::size_t expected_effects_amount = 2048;
	constexpr std::size_t particles_per_effect = 64;

	// MIN_UPDATE_PERIOD_MS; min interval between updates in milliseconds
	constexpr int update_rate = 10;

	// combination of these two should always equal to `expected_effects_amount`
	constexpr std::size_t cluster_amount = 64; // amount of threads
	constexpr std::size_t effects_per_cluster = 32; 

	// limits for request capacity of broker/cluster
	// smaller request buffers == less latency && more discraded calls
	constexpr std::size_t broker_request_buffer_size = std::max(cluster_amount, std::size_t(32)) * 2;
	constexpr std::size_t cluster_request_buffer_size = std::min(effects_per_cluster, std::size_t(8)) / 2;

	// acceleration factor of falling speed (> faster)
	constexpr float downward_acceleration = 0.0002f;

	// % chance for a single particle to explode
	constexpr uint8_t explosion_chance = 20;

	constexpr vec2_t particle_lifetime_range = { 1500, 2500 };

	constexpr bool colorful_particles = true;
	constexpr bool fading_particles = true;

	// Enable synchronization to remove visual jittering
	// Practically triples memory consumption and adds delay, hence optional
	constexpr bool triple_buffering = true;

	// How to copy staging buffer to back buffer
	constexpr cpy copy_method = cpy::memcpy;

	// window dimensions
	inline vec2_t dimensions;

}