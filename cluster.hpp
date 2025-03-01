#pragma once

#include <array>
#include <atomic>

#include "effect.hpp"
#include "bounded_buffer.hpp"
#include "vectors.hpp"
#include "particle.hpp"

struct Broker;

class Cluster {

public:

	using buffer_type = std::array<Effect, config::effects_per_cluster>;

	Cluster (Broker*);
	Cluster (const Cluster&) = delete;
	Cluster (Cluster&&) = delete;

	void update (const int dt);
	void render();

	inline void request_effect_at (vec2_t& coords) { requests.force_push(coords); }
	inline const std::size_t get_load() const { return requests.size() + active_effects_count; }

private:
	
	void stage_to_back();
	void swap_buffers_if_needed();
	void try_create_effect_at (vec2_t coords);

private:

	std::size_t active_effects_count = 0;

	std::atomic_bool can_swap = true;
	std::atomic_bool can_copy = true;

	std::size_t staging_timestamp = 0;
	std::size_t front_timestamp = 0;
	std::size_t back_timestamp = 0;

	buffer_type staging_buffer;

	std::unique_ptr<buffer_type> front_buffer = nullptr;
	std::unique_ptr<buffer_type> back_buffer = nullptr;

	bounded_buffer<vec2_t, config::cluster_request_buffer_size> requests;

	Broker* broker = nullptr;

};

