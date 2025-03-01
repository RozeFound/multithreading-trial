#include "particle.hpp"

#include "config.hpp"
#include "broker.hpp"
#include "random.hpp"

namespace platform {
	extern void drawPoint(float, float, float, float, float, float);
}

void Particle::update (const int dt) {

	age += dt;

	coords.x += velocity.x * dt;
	coords.y += velocity.y * dt;

	velocity.y -= config::downward_acceleration * dt;

	if constexpr (config::fading_particles)
		color.alpha = 1.f - static_cast<float>(age) / static_cast<float>(lifetime);

	bool out_of_bounds = 
		coords.x <= 0 || coords.y <= 0
		|| coords.x >= config::dimensions.width
		|| coords.y >= config::dimensions.height;

	if (out_of_bounds || age > lifetime)
		alive = false;

	if (!alive && !out_of_bounds) {
		bool should_explode = random::get_boolean(config::explosion_chance);
		if (should_explode) broker->request_effect_at(coords);
	}

}

void Particle::render() const {
	platform::drawPoint(coords.x, coords.y, color.r, color.g, color.b, color.a);
}
