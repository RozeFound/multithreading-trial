#pragma once

#include <vector>

#include "vectors.hpp"
#include "bounded_buffer.hpp"
#include "cluster.hpp"

struct Broker {

	Broker();

	inline void request_effect_at (vec2_t coords) { requests.force_push(coords); }

	bounded_buffer<vec2_t, config::broker_request_buffer_size> requests;
	
	std::vector<std::unique_ptr<Cluster>> clusters;


};