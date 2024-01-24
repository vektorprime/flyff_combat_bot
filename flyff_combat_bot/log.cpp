#include "log.h"

flyff::log::log(std::string file_name)
{
	file_name_ = std::move(file_name);
	log_file_.open(file_name_, std::ios::app);
	if (log_file_.fail())
	{
		std::cout << "Unable to open log file";
	}
	log_file_ << "Log file started \n";
}

void flyff::log::write(const std::string &message)
{
	log_file_ << message + "\n";
	log_file_.flush();
}


bool flyff::log::log_level_is_verbose()
{
	if (log_level_ == level::verbose)
	{
		return true;
	}

	return false;
}


void flyff::log::set_verbose_log_level()
{
	log_level_ = level::verbose;
}