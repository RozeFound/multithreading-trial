#pragma once

#include <array>

#include "particle.hpp"
#include "vectors.hpp"
#include "config.hpp"

class Cluster;

class Effect {

public:

	using buffer_type = std::array<Particle, config::particles_per_effect>;

	Effect() = default;
	Effect (const Effect& other) : particles(other.particles), depleted(other.depleted) {}

	void create_at (vec2_t coords);

	void update (const int dt);
	void render() const;

	bool depleted = true;
	Broker* broker = nullptr;
	buffer_type particles;

};