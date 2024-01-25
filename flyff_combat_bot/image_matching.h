// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#pragma once

//regular includes
#include <Windows.h>
#include <iostream>

//async
#include <thread>
#include <future>


//used for image matching
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"


//custom headers
#include "game_window.h"
#include "trigger_time.h"
#include "log.h"

namespace flyff
{
	struct match_result
	{
		double minVal = 1.0;
		cv::Point minLoc{};
		cv::Point maxLoc{};
		cv::Point matchLoc{};
		int result_cols = 0;
		int result_rows = 0;
		cv::Mat templ{};

		match_result() = default;

		match_result(const match_result &other) :
			minVal(other.minVal),
			minLoc(other.minLoc),
			maxLoc(other.maxLoc),
			matchLoc(other.matchLoc),
			result_cols(other.result_cols),
			result_rows(other.result_rows),
			templ(other.templ.clone())
		{
			std::cout << "match_result copy constructor invoked" << std::endl;
		}

		match_result &operator=(const match_result &other)
		{
			if (this != &other)
			{
				minVal = other.minVal;
				minLoc = other.minLoc;
				maxLoc = other.maxLoc;
				matchLoc = other.matchLoc;
				result_cols = other.result_cols;
				result_rows = other.result_rows;
				templ = other.templ.clone();  // Assuming cv::Mat.clone() for deep copy
				// Add a print statement for testing
				std::cout << "match_result copy assignment operator invoked" << std::endl;
			}
			return *this;
		}

		match_result(match_result &&other) noexcept :
			minVal(std::move(other.minVal)),
			minLoc(std::move(other.minLoc)),
			maxLoc(std::move(other.maxLoc)),
			matchLoc(std::move(other.matchLoc)),
			result_cols(std::move(other.result_cols)),
			result_rows(std::move(other.result_rows)),
			templ(std::move(other.templ))
		{
			std::cout << "match_result move constructor invoked" << std::endl;
		}

		match_result &operator=(match_result &&other) noexcept
		{
			if (this != &other)
			{
				minVal = std::move(other.minVal);
				minLoc = std::move(other.minLoc);
				maxLoc = std::move(other.maxLoc);
				matchLoc = std::move(other.matchLoc);
				result_cols = std::move(other.result_cols);
				result_rows = std::move(other.result_rows);
				templ = std::move(other.templ);
				std::cout << "match_result move assignment operator invoked" << std::endl;
			}
			return *this;
		}

		~match_result() = default;
	};


	match_result match_flyff_item(flyff::window &win, cv::Mat templ, cv::ColorConversionCodes color = cv::COLOR_BGRA2BGR);

	enum class type_of_match
	{
		char_screen,
		monster_target,
		combat_monster,
		party_window,
		combat,
		skillbar,
		alert_text,
		navigator_icon
	};

	match_result match_specific_flyff_item(flyff::window &win, cv::Mat templ, const flyff::type_of_match type_of_match);

	match_result match_flyff_item_alert_text(flyff::window &win, cv::Mat templ);

	match_result match_flyff_item_with_cursor(flyff::window &win, cv::Mat templ);

	match_result match_flyff_item_map_zones(flyff::window &win, cv::Mat templ, const int xoffset = 0, const int yoffset = 0);

	match_result match_flyff_item_combat_monster_focused(flyff::window &win, cv::Mat templ, int const last_xloc, const int last_yloc);

}





struct screen_cords
{
	int x = 0;
	int y = 0;
};

bool find_target_icons(flyff::window &win);

screen_cords get_target_to_click_cords(flyff::window &win);





struct Match_params
{
	cv::Mat img{};
	cv::Mat templ{};
	cv::Mat result{};
	int match_method = 1;
};

struct Simple_match_result
{
	double minVal = 1.0;
	cv::Point minLoc{};
	cv::Point maxLoc{};
	cv::Point matchLoc{};
	int result_cols = 0;
	int result_rows = 0;
};

//cv::Mat match_method_with_params(Match_params params);

//match_result parse_match_result(cv::Mat result);
//
//std::vector<match_result> match_flyff_items_async_with_window(cv::Mat img, cv::Mat monster_templ, cv::Mat aggro_templ, const int add_xcord = 0, const int add_ycord = 0);



void exit_monster_target(flyff::window &win, cv::Mat monster_target_templ);


int find_nav_angle(flyff::window &win);





enum class disconnect_message
{
	disconnected,
	cannot_connect,
	account_connected,
	connecting
};


bool check_if_disconnected(flyff::window &win, const disconnect_message &message, flyff::time &disconnected_time, flyff::match_result &disconnected_result);


namespace flyff 
{


	class image_match
	{
		public:
			struct result
			{
				double minval = 1.0;
				double maxval = 0.0;
				cv::Point matchloc{};
				cv::Point minloc{};
				cv::Point maxloc{};
				bool match_found = false;
			};
		
		private:

			double minval_ = 1.0;
			cv::Mat templ_{};
			cv::Point matchloc_{};


	};
}



void check_if_char_screen_icon_missing(flyff::window &win, flyff::time &char_scree_icon_time, flyff::log &log_file);
