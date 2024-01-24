// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#pragma once


#include <chrono>

using namespace std::chrono_literals;

class self_buff
{
public:
	char skill_key = '0';
	int skill_interval_int = 300;
	std::chrono::steady_clock::duration skill_interval = 300s;
	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
	
private:
	
};
