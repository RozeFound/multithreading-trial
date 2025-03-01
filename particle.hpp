#pragma once

#include "vectors.hpp"

struct Broker;

struct Particle {

	void update (const int dt);
	void render() const;

	bool alive = false;
	int lifetime = 0;
	int age = 0;

	vec2_t coords{};
	vec2_t velocity{};

	vec4_t color = vec4_t(1.f, 1.f, 1.f, 1.f);

	Broker* broker = nullptr;

};