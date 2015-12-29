#include "worker_config.h"

worker_config::worker_config ()
{
}

worker_config::worker_config (const YAML::Node &config)
{
	if (!config["broker"].IsScalar()) {
		throw config_error("Broker URI is not a scalar");
	}

	broker_uri_ = config["broker"].as<std::string>();

	if (!config["headers"].IsMap()) {
		throw config_error("Headers are not a map");
	}

	for (auto entry: config["headers"]) {
		if (!entry.first.IsScalar()) {
			throw config_error("A header key is not scalar");
		}

		if (entry.second.IsSequence()) {
			for (auto value: entry.second) {
				if (!value.IsScalar()) {
					throw config_error("A header value is not scalar");
				}

				headers_.insert(std::make_pair(
					entry.first.as<std::string>(),
					value.as<std::string>()
				));
			}
		} else if (entry.second.IsScalar()){
			headers_.insert(std::make_pair(
				entry.first.as<std::string>(),
				entry.second.as<std::string>()
			));
		} else {
			throw config_error("A header value is not scalar");
		}
	}
}

std::string worker_config::get_broker_uri () const
{
	return broker_uri_;
}

const worker_config::header_map_t &worker_config::get_headers () const
{
	return headers_;
}