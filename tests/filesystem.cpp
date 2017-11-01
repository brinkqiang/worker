#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/helpers/filesystem.h"

typedef std::tuple<std::string, std::string, sandbox_limits::dir_perm> bound_dirs_tuple;
typedef std::vector<bound_dirs_tuple> bound_dirs_type;


TEST(filesystem_test, normalize)
{
	ASSERT_EQ("", helpers::normalize_path(fs::path(".")).string());
	ASSERT_EQ("/", helpers::normalize_path(fs::path("/")).string());
	ASSERT_EQ("/first/second/third", helpers::normalize_path(fs::path("/first/second/third")).string());
	ASSERT_EQ("first/second/third", helpers::normalize_path(fs::path("first/second/third")).string());
	ASSERT_EQ("/second/third", helpers::normalize_path(fs::path("/first/../second/third")).string());
	ASSERT_EQ("", helpers::normalize_path(fs::path("../hello/hello/hello")).string());
	ASSERT_EQ("/third", helpers::normalize_path(fs::path("/first/second/../../third")).string());
	ASSERT_EQ("", helpers::normalize_path(fs::path("/first/second/../../../third")).string());
	ASSERT_EQ("", helpers::normalize_path(fs::path("first/second/../../../third")).string());
	ASSERT_EQ("/", helpers::normalize_path(fs::path("/././.")).string());
	ASSERT_EQ("/first/second/third", helpers::normalize_path(fs::path("/first/./second/third")).string());
	ASSERT_EQ("/first/second/third", helpers::normalize_path(fs::path("/first//second/third")).string());
	ASSERT_EQ("/first/third", helpers::normalize_path(fs::path("/first////second/../third")).string());
	ASSERT_EQ("/first/third", helpers::normalize_path(fs::path("/first////second/../third/././.")).string());
}


bound_dirs_type get_default_dirs()
{
	bound_dirs_type dirs;
	dirs.push_back(bound_dirs_tuple("/path/outside/sandbox", "/evaluate", sandbox_limits::dir_perm::RW));
	dirs.push_back(bound_dirs_tuple("/another/path/outside/sandbox", "/execute/dir", sandbox_limits::dir_perm::RW));
	return dirs;
}

TEST(filesystem_test, test_relative_1)
{
	bound_dirs_type dirs = get_default_dirs();
	fs::path result = helpers::find_path_outside_sandbox("inside", "chdir", dirs);
	ASSERT_EQ(fs::path().string(), result.string());
}

TEST(filesystem_test, test_relative_2)
{
	bound_dirs_type dirs = get_default_dirs();
	fs::path result = helpers::find_path_outside_sandbox("inside", "/evaluate", dirs);
	ASSERT_EQ((fs::path("/path/outside/sandbox") / fs::path("inside")).string(), result.string());
}

TEST(filesystem_test, test_relative_3)
{
	bound_dirs_type dirs = get_default_dirs();
	fs::path result = helpers::find_path_outside_sandbox("inside/file", "/evaluate", dirs);
	ASSERT_EQ((fs::path("/path/outside/sandbox") / fs::path("inside/file")).string(), result.string());
}

TEST(filesystem_test, test_absolute_1)
{
	bound_dirs_type dirs = get_default_dirs();
	fs::path result = helpers::find_path_outside_sandbox("/output.stderr", "/evaluate/test1", dirs);
	ASSERT_EQ(fs::path().string(), result.string());
}

TEST(filesystem_test, test_absolute_2)
{
	bound_dirs_type dirs = get_default_dirs();
	fs::path result = helpers::find_path_outside_sandbox("/evaluate/output.stderr", "/evaluate/test1", dirs);
	ASSERT_EQ((fs::path("/path/outside/sandbox") / fs::path("output.stderr")).string(), result.string());
}

TEST(filesystem_test, test_absolute_3)
{
	bound_dirs_type dirs = get_default_dirs();
	fs::path result = helpers::find_path_outside_sandbox("/evaluate/test1/output.stderr", "/evaluate/test1", dirs);
	ASSERT_EQ((fs::path("/path/outside/sandbox") / fs::path("test1/output.stderr")).string(), result.string());
}

TEST(filesystem_test, test_absolute_4)
{
	bound_dirs_type dirs = get_default_dirs();
	fs::path result = helpers::find_path_outside_sandbox("/evaluate/test1/sub/output.stderr", "/evaluate/test1", dirs);
	ASSERT_EQ((fs::path("/path/outside/sandbox") / fs::path("test1/sub/output.stderr")).string(), result.string());
}

TEST(filesystem_test, test_absolute_5)
{
	bound_dirs_type dirs = get_default_dirs();
	fs::path result = helpers::find_path_outside_sandbox("/execute/dir/sub/output.stderr", "/evaluate/test1", dirs);
	ASSERT_EQ((fs::path("/another/path/outside/sandbox") / fs::path("sub/output.stderr")).string(), result.string());
}

TEST(filesystem_test, test_relative_6)
{
	bound_dirs_type dirs = get_default_dirs();
	fs::path result = helpers::find_path_outside_sandbox("../inside", "/evaluate", dirs);
	ASSERT_EQ(fs::path().string(), result.string());
}

TEST(filesystem_test, test_absolute_7)
{
	bound_dirs_type dirs = get_default_dirs();
	fs::path result = helpers::find_path_outside_sandbox("../output.stderr", "/evaluate/test1", dirs);
	ASSERT_EQ((fs::path("/path/outside/sandbox") / fs::path("output.stderr")).string(), result.string());
}

TEST(filesystem_test, test_absolute_8)
{
	bound_dirs_type dirs = get_default_dirs();
	fs::path result =
		helpers::find_path_outside_sandbox("/evaluate/test1////sub/./output.stderr", "/evaluate/test1", dirs);
	ASSERT_EQ((fs::path("/path/outside/sandbox") / fs::path("test1/sub/output.stderr")).string(), result.string());
}
