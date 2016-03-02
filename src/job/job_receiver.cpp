#include "job_receiver.h"
#include "../connection_proxy.h"
#include "../eval_request.h"
#include "../eval_response.h"
#include "../helpers/zmq_socket.h"
#include "../commands/jobs_client_commands.h"


job_receiver::job_receiver(
	zmq::context_t &context, std::shared_ptr<job_evaluator> evaluator, std::shared_ptr<spdlog::logger> logger)
	: socket_(context, ZMQ_PAIR), evaluator_(evaluator), logger_(logger)
{
	if (logger == nullptr) {
		// Create logger manually to avoid global registration of logger
		auto sink = std::make_shared<spdlog::sinks::null_sink_st>();
		logger_ = std::make_shared<spdlog::logger>("job_receiver_nolog", sink);
	}

	// init depandent command structure
	job_client_context dependent_context = { evaluator_, socket_ };

	// init command structure
	commands_ = std::make_shared<command_holder<job_client_context>>(dependent_context, logger_);
	commands_->register_command("eval", jobs_client_commands::process_eval<job_client_context>);
}

void job_receiver::start_receiving()
{
	socket_.connect("inproc://" + JOB_SOCKET_ID);

	while (true) {
		logger_->info() << "Job-receiver: Waiting for incomings requests...";

		std::vector<std::string> message;
		bool terminate;
		if (!helpers::recv_from_socket(socket_, message, &terminate)) {
			if (terminate) {
				break;
			}
			logger_->warn() << "Job-receiver: failed to receive message. Skipping...";
			continue;
		}

		// Invoke command callback
		if (!message.empty()) {
			commands_->call_function(message[0], message);
		}
	}
}
