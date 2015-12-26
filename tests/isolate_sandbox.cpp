#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include "../src/sandbox/isolate_sandbox.h"

TEST(IsolateSandbox, BasicCreation)
{
	sandbox_limits limits;
	EXPECT_NO_THROW(isolate_sandbox s(limits, 34));
	isolate_sandbox is(limits, 34);
	EXPECT_EQ(is.get_dir(), "/tmp/box/34");
	EXPECT_THROW(isolate_sandbox s(limits, 2365), sandbox_exception);
}

TEST(IsolateSandbox, RunCommand)
{
	sandbox_limits limits;
	limits.wall_time = 5.1;
	limits.cpu_time = 5.1;
	limits.extra_time = 1;
	limits.disk_blocks = 500;
	limits.disk_inodes = 500;
	limits.memory_usage = 100000;
	limits.stdout = "output.txt";
	limits.stack_size = 0;
	limits.files_size = 0;
	limits.processes = 0;
	limits.share_net = false;
	isolate_sandbox *is;
	EXPECT_NO_THROW(is = new isolate_sandbox(limits, 34));
	EXPECT_EQ(is->get_dir(), "/tmp/box/34");
	//is->run("/usr/bin/ls", "-a");
	EXPECT_NO_THROW(is->run("/usr/bin/ls", std::vector<std::string>{"-a", "-l"}));
	fs::copy_file("/tmp/box/34/box/output.txt", "/tmp/output.txt");
	delete is;
}
