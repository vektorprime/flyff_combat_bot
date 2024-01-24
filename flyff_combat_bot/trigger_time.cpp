// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "trigger_time.h"

using namespace std::chrono_literals;
using time_point = std::chrono::steady_clock::time_point;
using duration = std::chrono::steady_clock::duration;


time_point flyff::time::get_start_time()
{
	return start_time_;
}

void flyff::time::set_start_time()
{
	start_time_ = std::chrono::steady_clock::now();
}

time_point flyff::time::get_current_time()
{
	return current_time_;
}

void flyff::time::set_current_time()
{
	current_time_ = std::chrono::steady_clock::now();
}

duration flyff::time::get_interval()
{
	return interval_;
}

void flyff::time::set_interval(int interval)
{
	interval_ = std::chrono::seconds(interval);
}

bool flyff::time::check_if_time_elapsed()
{
	set_current_time();
	if (current_time_ > start_time_ + interval_)
	{
		time_elapsed_ = true;
		return time_elapsed_;
	}
	else
	{
		time_elapsed_ = false;
		return time_elapsed_;
	}
}