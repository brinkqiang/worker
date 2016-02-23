#ifndef CODEX_WORKER_TASK_BASE_HPP
#define CODEX_WORKER_TASK_BASE_HPP

#include <vector>
#include <string>
#include <cstdlib>
#include <memory>
#include "../config/task_results.h"
#include "../config/task_metadata.h"


/**
 * Logical base of all possible tasks.
 * Its abstract class which cannot be instantiated.
 * But it stores some information about task which always have to be defined.
 */
class task_base
{
public:
	/**
	 * Default constructor deleted.
	 */
	task_base() = delete;
	/**
	 * Only possible way of construction which just store all given parameters into private variables.
	 * @param id unique identificator of load order of tasks
	 * @param task_meta variable containing further info about task
	 */
	task_base(size_t id, task_metadata task_meta);
	/**
	 * Virtual destructor.
	 */
	virtual ~task_base();

	/**
	 * This method runs operation which this task is supposed to do.
	 * @return results to be pushed back
	 */
	virtual std::shared_ptr<task_results> run() = 0;
	/**
	 * Add child to this task. Once given child cannot be deleted.
	 * @param add
	 */
	void add_children(std::shared_ptr<task_base> add);
	/**
	 * Get all children of task.
	 * @return constant reference to vector of task_base pointers
	 */
	const std::vector<std::shared_ptr<task_base>> &get_children();
	/**
	 * To this task object add parent.
	 * @param add
	 */
	void add_parent(std::shared_ptr<task_base> add);

	/**
	 * Get unique identification number.
	 * This number expresses order in job configuration.
	 * @return unsigned integer
	 */
	size_t get_id();
	/**
	 * Unique task ID which was stated in job configuration.
	 * @return textual description of id
	 */
	const std::string &get_task_id();
	/**
	 * Get priority of this task.
	 * Lower number -> higher priority.
	 * @return unsigned integer
	 */
	size_t get_priority();
	/**
	 * If true than failure of this task will cause immediate exit of job evaluation.
	 * @return
	 */
	bool get_fatal_failure();
	/**
	 * Get command which will be executed within this task.
	 * @return textual description of command
	 */
	const std::string &get_cmd();
	/**
	 * Arguments which will be supplied to executed command.
	 * @return constant vector of texts
	 */
	const std::vector<std::string> &get_args();
	/**
	 * Return dependencies of this particular task.
	 * @return textual vector array of dependencies
	 */
	const std::vector<std::string> &get_dependencies();

	bool is_executable();
	void set_execution(bool set);
	void set_children_execution(bool set);

protected:
	size_t id_;
	task_metadata task_meta_;
	bool execute_;

	std::vector<std::weak_ptr<task_base>> parents_;
	std::vector<std::shared_ptr<task_base>> children_;
};


/**
 * Task exception class.
 */
class task_exception : public std::exception
{
public:
	/**
	 * Generic task exception with no specification.
	 */
	task_exception() : what_("Generic task exception")
	{
	}
	/**
	 * Exception with some brief description.
	 * @param what textual description of a problem
	 */
	task_exception(const std::string &what) : what_(what)
	{
	}
	/**
	 * Virtual destructor.
	 */
	virtual ~task_exception()
	{
	}
	/**
	 * Return description of this exception.
	 * @return C string
	 */
	virtual const char *what() const noexcept
	{
		return what_.c_str();
	}

protected:
	std::string what_;
};


/**
 * Comparator which is used for topological sorting of tasks
 */
class task_compare
{
public:
	/**
	 * Greater than operator on task_base objects.
	 * @param a first parameter
	 * @param b second parameter
	 * @return true if parameter a is greater than b
	 */
	bool operator()(std::shared_ptr<task_base> a, std::shared_ptr<task_base> b)
	{
		if (a->get_priority() > b->get_priority()) {
			return true;
		} else if (a->get_priority() == b->get_priority() && a->get_id() > b->get_id()) {
			return true;
		}

		return false;
	}
};

#endif // CODEX_WORKER_TASK_BASE_HPP
