#include "effect.hpp"

#include "random.hpp"

void Effect::create_at (vec2_t coords) {

	for (auto& p : particles) {

		p.age = 0;
		p.alive = true;
		p.broker = broker;

		p.lifetime = random::get_int((int)config::particle_lifetime_range.lower,
									 (int)config::particle_lifetime_range.upper);
		p.coords = coords;

		p.velocity.y = random::get_full_range_float() / 10.f;
		p.velocity.x = random::get_full_range_float() / 10.f;

		if constexpr (config::colorful_particles) {
			p.color.r = random::get_float();
			p.color.g = random::get_float();
			p.color.b = random::get_float();
		}

		if constexpr (config::fading_particles)
			p.color.alpha = 1.f;
	}

	depleted = false;

}

void Effect::update (const int dt) {

	bool has_alive_particles = false;

	for (auto& particle : particles) {

		if (!particle.alive) continue;

		particle.update(dt);

		if (particle.alive)
			has_alive_particles = true;

	}

	if (!has_alive_particles)
		depleted = true;

}

void Effect::render() const {

	for (const auto& particle : particles) {

		if (!particle.alive) continue;

		particle.render();

	}

}
