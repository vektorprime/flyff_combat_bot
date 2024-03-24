// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "map_image_matching.h"

#include "image_matching.h"
#include "mouse_and_keyboard.h"

void flyff::image_match_map::scan_single_map_zone_(flyff::window &win, cv::Mat templ, const map quadrant)
{

	//double minVal = 0.0; 
	//double maxVal = 0.0; cv::Point minLoc; cv::Point maxLoc;
	//cv::Point matchLoc;

	//offset for top left of map is 130, 165.
	//offset for top right of map is 95, 165
	//offset for bottom left of map is 130, 195
	//offset for bottom right of map is 95, 195
	int xoffset = 0;
	int yoffset = 0;
	switch (quadrant)
	{
	case map::top_left:
		xoffset = 130;
		yoffset = 165;
		break;
	case map::top_right:
		xoffset = 95;
		yoffset = 165;
		break;
	case map::bottom_left:
		xoffset = 130;
		yoffset = 195;
		break;
	case map::bottom_right:
		xoffset = 95;
		yoffset = 195;
		break;
	}

	cv::Mat result, img, img2;
	RECT supportwindowrect_size;
	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	img = win.get_mat();
	int windowxcord = static_cast<int>(supportwindowrect_size.right - xoffset);
	int windowycord = yoffset;
	int windowwidth = 30;
	int windowheight = 30;
	x_y_cords cords = check_x_y_cords(windowxcord, windowycord, windowwidth, windowheight, supportwindowrect_size);
	img2 = img(cv::Rect(cords.windowxcord, cords.windowycord, cords.windowwidth, cords.windowheight));
	cv::cvtColor(img2, img2, cv::COLOR_BGRA2BGR);
	int result_cols = img2.cols - templ.cols + 1;
	int result_rows = img2.rows - templ.rows + 1;
	result.create(result_rows, result_cols, CV_8UC4);
	matchTemplate(img2, templ, result, cv::TM_SQDIFF_NORMED);
	switch (quadrant)
	{
	case map::top_left:
		minMaxLoc(result, &top_left_map_result_.minval, &top_left_map_result_.maxval, &top_left_map_result_.minloc, &top_left_map_result_.maxloc, cv::Mat());
		top_left_map_result_.matchloc = top_left_map_result_.minloc;

		break;
	case map::top_right:
		minMaxLoc(result, &top_right_map_result_.minval, &top_right_map_result_.maxval, &top_right_map_result_.minloc, &top_right_map_result_.maxloc, cv::Mat());
		top_right_map_result_.matchloc = top_right_map_result_.minloc;
		break;
	case map::bottom_left:
		minMaxLoc(result, &bottom_left_map_result_.minval, &bottom_left_map_result_.maxval, &bottom_left_map_result_.minloc, &bottom_left_map_result_.maxloc, cv::Mat());
		bottom_left_map_result_.matchloc = bottom_left_map_result_.minloc;

		break;
	case map::bottom_right:
		minMaxLoc(result, &bottom_right_map_result_.minval, &bottom_right_map_result_.maxval, &bottom_right_map_result_.minloc, &bottom_right_map_result_.maxloc, cv::Mat());
		bottom_right_map_result_.matchloc = bottom_right_map_result_.minloc;
		break;
	}
	//matchLoc = minLoc;
}


void flyff::image_match_map::scan_map_zones(flyff::window &win, cv::Mat templ)
{
	
	flyff::image_match_map::scan_single_map_zone_(win, templ, map::top_left);
	flyff::image_match_map::scan_single_map_zone_(win, templ, map::top_right);
	flyff::image_match_map::scan_single_map_zone_(win, templ, map::bottom_left);
	flyff::image_match_map::scan_single_map_zone_(win, templ, map::bottom_right);

}

bool flyff::image_match_map::match_found_in_top_left()
{
	if (top_left_map_result_.minval < cutoff_point_)
	{
		top_left_map_result_.match_found = true;
	}

	return top_left_map_result_.match_found;
}

bool flyff::image_match_map::match_found_in_top_right()
{
	if (top_right_map_result_.minval < cutoff_point_)
	{
		top_right_map_result_.match_found = true;
	}

	return top_right_map_result_.match_found;
}

bool flyff::image_match_map::match_found_in_bottom_left()
{
	if (bottom_left_map_result_.minval < cutoff_point_)
	{
		bottom_left_map_result_.match_found = true;
	}

	return bottom_left_map_result_.match_found;
}

bool flyff::image_match_map::match_found_in_bottom_right()
{
	if (bottom_right_map_result_.minval < cutoff_point_)
	{
		bottom_right_map_result_.match_found = true;
	}

	return bottom_right_map_result_.match_found;
}

void avoid_giant_on_map(flyff::window &win, const flyff::process &target_char_position)
{
	//std::cout << "checking for giant\n";

	cv::Mat monster_target_templ = cv::imread("images\\monstertarget.png");
	cv::cvtColor(monster_target_templ, monster_target_templ, cv::COLOR_BGRA2BGR);

	cv::Mat giant_on_map_templ = cv::imread("images\\giantonmap.png");
	cv::cvtColor(giant_on_map_templ, giant_on_map_templ, cv::COLOR_BGRA2BGR);

	flyff::image_match_map giant_on_map_result;
	giant_on_map_result.scan_map_zones(win, giant_on_map_templ);

	if (giant_on_map_result.match_found_in_top_left())
	{
		std::cout << "found giant or violet on top left of map\n";
			set_precise_player_angle(45.0f, target_char_position);
			send_key_hold_down(0x57, 2000);
			exit_monster_target(win, monster_target_templ);
	}

	if (giant_on_map_result.match_found_in_top_right())
	{
		std::cout << "found giant or violet on top right of map\n";

			set_precise_player_angle(315.0f, target_char_position);
			send_key_hold_down(0x57, 2000);
			exit_monster_target(win, monster_target_templ);
	}

	if (giant_on_map_result.match_found_in_bottom_left())
	{
		std::cout << "found giant or violet on bottom left of map\n";

			set_precise_player_angle(135.0f, target_char_position);
			send_key_hold_down(0x57, 2000);
			exit_monster_target(win, monster_target_templ);		
	}

	if (giant_on_map_result.match_found_in_bottom_right())
	{
		std::cout << "found giant or violet on bottom right of map\n";
	
			set_precise_player_angle(225.0f, target_char_position);
			send_key_hold_down(0x57, 2000);
			exit_monster_target(win, monster_target_templ);		
	}
}


void flyff::player::avoid_giant_on_map(flyff::window &win)
{
	if (log_file_->log_level_is_verbose())
	{
		log_file_->write("Starting avoid_giant_on_map");
	}
	//std::cout << "checking for giant\n";

	cv::Mat monster_target_templ = cv::imread("images\\monstertarget.png");
	cv::cvtColor(monster_target_templ, monster_target_templ, cv::COLOR_BGRA2BGR);

	cv::Mat giant_on_map_templ = cv::imread("images\\giantonmap.png");
	cv::cvtColor(giant_on_map_templ, giant_on_map_templ, cv::COLOR_BGRA2BGR);

	flyff::image_match_map giant_on_map_result;
	giant_on_map_result.scan_map_zones(win, giant_on_map_templ);

	if (giant_on_map_result.match_found_in_top_left())
	{
		if (log_file_->log_level_is_verbose())
		{
			log_file_->write("Found giant or violet on top left of map");
		}
		std::cout << "found giant or violet on top left of map\n";
		set_angle(45.0f);
		//send_key_hold_down(0x57, 2000);
		send_2_keys_and_hold_first(0x57, 2000, VK_SPACE);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		exit_monster_target(win, monster_target_templ);
	}

	if (giant_on_map_result.match_found_in_top_right())
	{
		if (log_file_->log_level_is_verbose())
		{
			log_file_->write("Found giant or violet on top right of map");
		}
		std::cout << "Found giant or violet on top right of map\n";

		set_angle(315.0f);
		send_2_keys_and_hold_first(0x57, 2000, VK_SPACE);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		exit_monster_target(win, monster_target_templ);
	}

	if (giant_on_map_result.match_found_in_bottom_left())
	{
		if (log_file_->log_level_is_verbose())
		{
			log_file_->write("Found giant or violet on bottom left of map");
		}
		std::cout << "Found giant or violet on bottom left of map\n";
		set_angle(135.0f);
		send_2_keys_and_hold_first(0x57, 2000, VK_SPACE);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		exit_monster_target(win, monster_target_templ);
	}

	if (giant_on_map_result.match_found_in_bottom_right())
	{
		if (log_file_->log_level_is_verbose())
		{
			log_file_->write("Found giant or violet on bottom right of map");
		}
		std::cout << "Found giant or violet on bottom right of map\n";
		set_angle(225.0f);
		send_2_keys_and_hold_first(0x57, 2000, VK_SPACE);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		exit_monster_target(win, monster_target_templ);
	}
}


void check_if_navigator_icon_missing(flyff::window &win, flyff::time &navigator_icon_time, flyff::log &log_file)
{

	if (navigator_icon_time.check_if_time_elapsed())
	{
		if (log_file.log_level_is_verbose())
		{
			log_file.write("Starting check_if_navigator_icon_missing");
		}
		const float navigator_icon_cutoffpoint = 0.009;
		cv::Mat navigator_icon_templ = cv::imread("images\\navigator_icon.png");
		cv::cvtColor(navigator_icon_templ, navigator_icon_templ, cv::COLOR_BGRA2BGR);
		flyff::match_result navigator_icon_result = match_specific_flyff_item(win, navigator_icon_templ, flyff::type_of_match::navigator_icon);
		if (navigator_icon_result.minVal > navigator_icon_cutoffpoint)
		{
			if (log_file.log_level_is_verbose())
			{
				log_file.write("Unable to find navigator icon, sending N key");
			}
			std::cout << "Unable to find navigator icon, sending N key\n";
			send_key_hold_down(0x4E);
		}
		navigator_icon_time.set_start_time();
	}
}