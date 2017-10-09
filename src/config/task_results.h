#ifndef RECODEX_WORKER_TASK_RESULTS_H
#define RECODEX_WORKER_TASK_RESULTS_H

#include <string>
#include <memory>

/**
 * Return error codes of sandbox. Code names corresponds isolate's meta file error codes.
 */
enum class isolate_status { OK, RE, SG, TO, XX, UNDEF };

/**
 * Status of whole task after execution.
 */
enum class task_status { OK, FAILED, SKIPPED };


/**
 * Sandbox results.
 * @note Not all items must be returned from sandbox, so some defaults may aply.
 */
struct sandbox_results {
	/**
	 * Return code of sandbox.
	 * Default: -1
	 */
	int exitcode;
	/**
	 * Total run time of program inside the sandbox.
	 * Default: 0 (s)
	 */
	float time;
	/**
	 * Total run time (wall clock) of program inside the sandbox.
	 * Default: 0 (s)
	 */
	float wall_time;
	/**
	 * Amount of memory used by program inside the sandbox.
	 * Default: 0 (kB)
	 */
	size_t memory;
	/**
	 * Maximum resident set size of the process.
	 * Default: 0 (kB)
	 */
	size_t max_rss;
	/**
	 * Error code returned by sandbox.
	 * Default: UNDEF
	 */
	isolate_status status;
	/**
	 * Signal, which killed the process.
	 * Default: 0
	 */
	int exitsig;
	/**
	 * Flag if program exited normaly or was killed.
	 * Default: false
	 */
	bool killed;
	/**
	 * Error message of the sandbox.
	 * Default: ""
	 */
	std::string message;
	/**
	 * Output as taken from stdout and stderr.
	 */
	std::string output;

	/**
	 * Constructor with default values initialization.
	 */
	sandbox_results()
		: exitcode(-1), time(0), wall_time(0), memory(0), max_rss(0), status(isolate_status::UNDEF), exitsig(0),
		  killed(false), message(), output()
	{
	}

	/**
	 * Defaulted copy constructor.
	 */
	sandbox_results(const sandbox_results &) = default;
	/**
	 * Defaulted move constructor.
	 */
	sandbox_results(sandbox_results &&) = default;
	/**
	 * Defaulted copy assignment.
	 */
	sandbox_results &operator=(const sandbox_results &) = default;
	/**
	 * Defaulted move assignment.
	 */
	sandbox_results &operator=(sandbox_results &&) = default;
};


/**
 * Generic task result. Common for all tasks (internal and external).
 */
struct task_results {
	/**
	 * Status of task after execution.
	 * Default: OK
	 */
	task_status status;
	/**
	 * Error message if the task failed.
	 * Default: ""
	 */
	std::string error_message;
	/**
	 * Pointer to @ref sandbox_results for external task results.
	 * Default: nullptr (other types of tasks)
	 */
	std::unique_ptr<sandbox_results> sandbox_status;

	/**
	 * Constructor with default values initiazation.
	 */
	task_results() : status(task_status::OK), error_message(), sandbox_status(nullptr)
	{
	}

	/**
	 * Defaulted copy constructor.
	 */
	task_results(const task_results &) = default;
	/**
	 * Defaulted move constructor.
	 */
	task_results(task_results &&) = default;
	/**
	 * Defaulted copy assignment.
	 */
	task_results &operator=(const task_results &) = default;
	/**
	 * Defaulted move assignment.
	 */
	task_results &operator=(task_results &&) = default;
};


#endif // RECODEX_WORKER_TASK_RESULTS_H
