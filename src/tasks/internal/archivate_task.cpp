#include "archivate_task.h"
#include "../../archives/archivator.h"


archivate_task::archivate_task(size_t id, std::shared_ptr<task_metadata> task_meta) : task_base(id, task_meta)
{
	if (task_meta_->cmd_args.size() != 2) {
		throw task_exception(
			"Wrong number of arguments. Required: 2, Actual: " + std::to_string(task_meta_->cmd_args.size()));
	}
}


archivate_task::~archivate_task()
{
}


std::shared_ptr<task_results> archivate_task::run()
{
	std::shared_ptr<task_results> result(new task_results());

	try {
		archivator::compress(task_meta_->cmd_args[0], task_meta_->cmd_args[1]);
	} catch (archive_exception &e) {
		result->status = task_status::FAILED;
		result->error_message = std::string("Cannot create archive. Error: ") + e.what();
	}

	return result;
}
