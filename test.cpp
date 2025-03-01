
#include "test.h"

#include <thread>
#include <atomic>
#include <cassert>

#include "./nvToolsExt.h"

#include "config.hpp"
#include "broker.hpp"

static std::atomic_int global_time;
static std::atomic_bool should_exit = false;
static std::unique_ptr<Broker> broker = nullptr;

void broker_thread() {

	while (!should_exit) {

		auto coords = broker->requests.pop_wait();

		std::size_t min_load = std::numeric_limits<std::size_t>::max();
		std::size_t cluster_id = 0;
		
		for (std::size_t i = 0; i < broker->clusters.size(); i++) {

			const auto& cluster = broker->clusters.at(i);
			const auto load = cluster->get_load();
			
			if (load < min_load) {
				min_load = load;
				cluster_id = i;
			}

		}

		broker->clusters.at(cluster_id)->request_effect_at(coords);

	}

}

void worker_thread (std::size_t cluster_id) {

	auto& cluster = broker->clusters.at(cluster_id);

	while (!should_exit) {

		nvtxRangePush(__FUNCTION__);

		thread_local int last_time = 0;

		const int time = global_time.load();
		const int delta = time - last_time;
		last_time = time;

		if (delta > 0)		
			cluster->update(delta);

		if (delta < config::update_rate) {
			auto sleep_time = std::chrono::milliseconds(config::update_rate - delta);
			std::this_thread::sleep_for(sleep_time);
		}

		nvtxRangePop();

	}

}


void test::init() {

	config::dimensions.width = SCREEN_WIDTH;
	config::dimensions.height = SCREEN_HEIGHT;

	static_assert(config::effects_per_cluster * config::cluster_amount == config::expected_effects_amount);
	static_assert(config::explosion_chance >= 0 && config::explosion_chance <= 100);

	broker = std::make_unique<Broker>();

	std::thread(broker_thread).detach();

	for (std::size_t i = 0; i < config::cluster_amount; i++)
		std::thread(worker_thread, i).detach();

}

void test::term() {
	should_exit = true;
	broker.reset();
}

void test::render() {
	for (auto& cluster : broker->clusters)
		cluster->render();
}

void test::update (int dt) {
	global_time.fetch_add(dt);
}

void test::on_click (int x, int y) {
	broker->request_effect_at({ static_cast<float>(x), static_cast<float>(config::dimensions.height - y) });
}