#include "external_task.h"

external_task::external_task(size_t id, const std::string &task_id, size_t priority, bool fatal, const std::string &log,
							 const std::vector<std::string> &dependencies,
							 const std::string &binary, const std::vector<std::string> &arguments,
							 const std::string &sandbox, sandbox_limits limits)
	: task_base(id, task_id, priority, fatal, dependencies, binary, arguments, log),
	  cmd_(binary), sandbox_id_(sandbox), limits_(limits)
{
	sandbox_check();
}

external_task::~external_task()
{}

void external_task::sandbox_check()
{
	bool found = false;

#ifndef _WIN32
	if (sandbox_id_ == "isolate") {
		found = true;
	}
#endif

	if (found == false) {
		throw task_exception("Unknown sandbox type: " + sandbox_id_);
	}
}

void external_task::sandbox_init()
{
#ifndef _WIN32
	if (sandbox_id_ == "isolate") {
		// TODO: isolate uid not given (its constant 1)
		sandbox_ = std::make_shared<isolate_sandbox>(limits_, 1);
	}
#endif
}

void external_task::sandbox_fini()
{
	sandbox_ = nullptr;
}

void external_task::run()
{
	// TODO: only temporary solution, should be removed
	if (sandbox_ == nullptr) { return; }

	sandbox_init();
	sandbox_->run(cmd_, arguments_);
	sandbox_fini();
	return;
}
