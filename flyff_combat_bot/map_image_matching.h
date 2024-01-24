// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#pragma once

#include "map_image_matching.h"

#include "game_window.h"
#include "memory_reader.h"
#include "image_matching.h"
#include "log.h"

namespace flyff
{

	class image_match_map : public image_match
	{
	public:

		void scan_map_zones(flyff::window &win, cv::Mat templ);
		bool match_found_in_top_left();
		bool match_found_in_top_right();
		bool match_found_in_bottom_left();
		bool match_found_in_bottom_right();

	private:

		enum class map
		{
			top_left,
			top_right,
			bottom_left,
			bottom_right,
		};

		void scan_single_map_zone_(flyff::window &win, cv::Mat templ, const map quadrant);
		result top_left_map_result_, top_right_map_result_, bottom_left_map_result_, bottom_right_map_result_;
		double cutoff_point_ = 0.004;
	};
}

//void avoid_giant_on_map(flyff::window &win, const flyff::process &target_char_position);

void check_if_navigator_icon_missing(flyff::window &win, flyff::time &navigator_icon_time, flyff::log &log_file);
