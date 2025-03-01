#include "cluster.hpp"

#include "effect.hpp"
#include "config.hpp"

void init_buffer (Broker* broker, Cluster::buffer_type* buffer, std::size_t size) {

	for (std::size_t i = 0; i < size; i++)
		buffer->at(i).broker = broker;

}

void copy_buffer (Cluster::buffer_type* from_buf, Cluster::buffer_type* to_buf) {

	for (std::size_t i = 0; i < config::effects_per_cluster; i++) {

		to_buf->at(i).depleted = from_buf->at(i).depleted;

		for (std::size_t j = 0; j < config::particles_per_effect; j++) {

			auto& lhs = to_buf->at(i).particles.at(j);
			auto& rhs = from_buf->at(i).particles.at(j);

			lhs.age = rhs.age;
			lhs.alive = rhs.alive;
			lhs.lifetime = rhs.lifetime;

			lhs.coords = rhs.coords;
			lhs.velocity = rhs.velocity;
			lhs.color = rhs.color;

		}

	}

}

Cluster::Cluster (Broker* broker) : broker(broker) {

	init_buffer(broker, &staging_buffer, config::effects_per_cluster);

	if constexpr (config::triple_buffering) {

		front_buffer = std::make_unique<buffer_type>();
		init_buffer(broker, front_buffer.get(), config::effects_per_cluster);

		back_buffer = std::make_unique<buffer_type>();
		init_buffer(broker, back_buffer.get(), config::effects_per_cluster);

	}

}

void Cluster::update (const int dt) {

	staging_timestamp += dt;

	for (auto& effect : staging_buffer) {

		if (effect.depleted) continue;

		effect.update(dt);

		if (effect.depleted)
			active_effects_count--;

	}

	vec2_t coords;

	while (requests.try_pop(coords))
		try_create_effect_at(coords);

	if constexpr (config::triple_buffering)
		stage_to_back();

}

void Cluster::render() {

	if constexpr (config::triple_buffering)
		swap_buffers_if_needed();

	buffer_type* buffer = &staging_buffer;

	if constexpr (config::triple_buffering)
		buffer = front_buffer.get();

	for (const auto& effect : *buffer)
		if (!effect.depleted) effect.render();

}

void Cluster::stage_to_back() {

	if (!can_copy.load()) return;

	can_swap.store(false);

	if constexpr (config::copy_method == cpy::assign)
		*back_buffer = staging_buffer;
	if constexpr (config::copy_method == cpy::function)
		copy_buffer(&staging_buffer, back_buffer.get());
	if constexpr (config::copy_method == cpy::std_copy)
		std::copy(staging_buffer.begin(), staging_buffer.end(), back_buffer->begin());
	if constexpr (config::copy_method == cpy::memcpy)
		std::memcpy(back_buffer->data(), staging_buffer.data(), staging_buffer.size() * sizeof(Effect));

	back_timestamp = staging_timestamp;

	can_swap.store(true);

}

void Cluster::swap_buffers_if_needed() {

	if (!can_swap.load()) return;

	can_copy.store(false);

	if (back_timestamp > front_timestamp) {
		std::swap(back_buffer, front_buffer);
		std::swap(back_timestamp, front_timestamp);
	}

	can_copy.store(true);

}

void Cluster::try_create_effect_at (vec2_t coords) {

	for (auto& effect : staging_buffer) {
		
		if (!effect.depleted) continue;
		
		effect.create_at(coords);
		active_effects_count++;
		
		break;
	
	}

}