// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#pragma once

////regular includes
#include <Windows.h>
//#include <iostream>

//used for image matching
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
//#include <opencv2/highgui/highgui.hpp>

struct x_y_cords
{
	int windowxcord = 0;
	int windowycord = 0;
	int windowwidth = 0;
	int windowheight = 0;
	RECT supportwindowrect_size{};
};

x_y_cords check_x_y_cords(int windowxcord, int windowycord, int windowwidth, int windowheight, RECT supportwindowrect_size);

struct center_window_data
{
	int win_xcord = 0;
	int win_ycord = 0;
	cv::Mat img{};
};


namespace flyff
{
	class window
	{
		public:
			cv::Mat get_mat(const bool cursor = false);
			cv::Mat get_and_convert_mat(cv::ColorConversionCodes color = cv::COLOR_BGRA2BGR);
			center_window_data get_mat_from_center_of_window(const double x, const double y, const double w, const double h);
			void free();
			//x_y_cords check_x_y_cords(int windowxcord, int windowycord, int windowwidth, int windowheight, RECT supportwindowrect_size);

		private:
			cv::Mat window_;
			center_window_data center_window_data_;
			
	};
}

struct Flyff_screen_cords
{
	int start_x = 0;
	int start_y = 0;
	int end_x = 0;
	int end_y = 0;
};

Flyff_screen_cords get_flyff_char_screen_cords();

Flyff_screen_cords get_flyff_map_cords();

int get_bottom_of_screen_y_cord();





