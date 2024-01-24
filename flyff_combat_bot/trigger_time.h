// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#pragma once
#include <chrono>



namespace flyff 
{
	using namespace std::chrono_literals;
	using time_point = std::chrono::steady_clock::time_point;
	using duration = std::chrono::steady_clock::duration;

	class time
	{
		public:
			time_point get_start_time();
			void set_start_time();

			time_point get_current_time();
			void set_current_time();

			duration get_interval();
			void set_interval(int interval);

			bool check_if_time_elapsed();

		private:
			time_point start_time_ = std::chrono::steady_clock::now();
			time_point current_time_ = std::chrono::steady_clock::now();
			duration interval_ = 60s;
			bool time_elapsed_ = true;
	};
}