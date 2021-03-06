#include "prefixed_file_manager.h"

prefixed_file_manager::prefixed_file_manager(std::shared_ptr<file_manager_interface> fm, const std::string &prefix)
	: prefix_(prefix), fm_(fm)
{
}

void prefixed_file_manager::get_file(const std::string &src_name, const std::string &dst_name)
{
	fm_->get_file(prefix_ + src_name, dst_name);
}

void prefixed_file_manager::put_file(const std::string &src_name, const std::string &dst_name)
{
	fm_->put_file(src_name, prefix_ + dst_name);
}
