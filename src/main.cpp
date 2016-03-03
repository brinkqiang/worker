#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "worker_core.h"


int main(int argc, char **argv)
{
	std::vector<std::string> args(argv, argv + argc);
	try {
		worker_core core(args);
		core.run();
	} catch (...) {
		std::cerr << "Something very bad happend!" << std::endl;
		return 1;
	}

	return 0;
}
