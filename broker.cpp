#include "broker.hpp"

#include "config.hpp"

Broker::Broker() {

	clusters.reserve(config::cluster_amount);

	for (std::size_t i = 0; i < clusters.capacity(); i++)
		clusters.emplace_back(std::make_unique<Cluster>(this));

};