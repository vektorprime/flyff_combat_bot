// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "debug_image_matching.h"
#include "image_matching.h"

// These are functions I created to debug the OpenCV match template function for various parts of the game window. Usually I pass the return cv::Mat to the cv::imshow function so I can see the debug window.

cv::Mat debug_map(flyff::window &win, const int xoffset, const int yoffset)
{
	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	RECT supportwindowrect_size{};
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	int windowxcord = static_cast<int>(supportwindowrect_size.right - xoffset);
	int windowycord = yoffset;
	int windowwidth = 30;
	int windowheight = 30;
	x_y_cords cords = check_x_y_cords(windowxcord, windowycord, windowwidth, windowheight, supportwindowrect_size);
	cv::Mat img = win.get_mat();
	cv::Mat img2 = img(cv::Rect(cords.windowxcord, cords.windowycord, cords.windowwidth, cords.windowheight));
	cv::cvtColor(img2, img2, cv::COLOR_BGRA2BGR);
	return img2;
}

cv::Mat debug_window(flyff::window &win, const double xoffset, const double yoffset, int windowwidth, int windowheight)
{

	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	RECT supportwindowrect_size{};
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	int windowxcord = static_cast<int>(supportwindowrect_size.right - xoffset);
	int windowycord = yoffset;
	if (windowxcord + windowwidth > supportwindowrect_size.right)
		windowwidth = supportwindowrect_size.right - windowxcord;
	if (windowycord + windowheight > supportwindowrect_size.bottom)
		windowheight = supportwindowrect_size.bottom - windowycord;
	cv::Mat img = win.get_mat();
	cv::Mat img2 = img(cv::Rect(windowxcord, windowycord, windowwidth, windowheight));
	cv::cvtColor(img2, img2, cv::COLOR_BGRA2BGR);
	return img2;
}

cv::Mat debug_hp_mp_fp(flyff::window &win, const double x, const double y, const double width, const double height)
{


	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	RECT supportwindowrect_size{};
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	int xoffset = static_cast<int>(supportwindowrect_size.right * x);
	int yoffset = static_cast<int>(supportwindowrect_size.bottom * y);
	int windowwidth = static_cast<int>(supportwindowrect_size.right * width);
	int windowheight = static_cast<int>(supportwindowrect_size.right * height);
	cv::Mat img = win.get_mat();
	cv::Mat img2 = img(cv::Rect(xoffset, yoffset, windowwidth, windowheight));
	cv::cvtColor(img2, img2, cv::COLOR_BGRA2BGR);


	cv::Mat mp_templ = cv::imread("images\\mp.png");
	cv::cvtColor(mp_templ, mp_templ, cv::COLOR_BGRA2BGR);

	cv::Mat hp_templ = cv::imread("images\\hp.png");
	cv::cvtColor(hp_templ, hp_templ, cv::COLOR_BGRA2BGR);

	cv::Mat fp_templ = cv::imread("images\\fp.png");
	cv::cvtColor(fp_templ, fp_templ, cv::COLOR_BGRA2BGR);


	double monitor_hp_mp_fp_cutoffpoint = 0.02;
	flyff::match_result monitor_hp_result = match_specific_flyff_item(win, hp_templ, flyff::type_of_match::char_screen);
	flyff::match_result monitor_mp_result = match_specific_flyff_item(win, mp_templ, flyff::type_of_match::char_screen);
	flyff::match_result monitor_fp_result = match_specific_flyff_item(win, fp_templ, flyff::type_of_match::char_screen);
	std::cout << "the minvals for hp, mp, and fp are " << monitor_hp_result.minVal << " " << monitor_mp_result.minVal << " " << monitor_fp_result.minVal << std::endl;
	cv::Mat img_display_hp, img_display_fp, img_display_mp;
	if (monitor_hp_result.minVal < monitor_hp_mp_fp_cutoffpoint) rectangle(img2, monitor_hp_result.matchLoc, cv::Point(monitor_hp_result.matchLoc.x + hp_templ.cols, monitor_hp_result.matchLoc.y + hp_templ.rows), cv::Scalar(0, 0, 255), 2, 8, 0);
	if (monitor_fp_result.minVal < monitor_hp_mp_fp_cutoffpoint) rectangle(img2, monitor_fp_result.matchLoc, cv::Point(monitor_fp_result.matchLoc.x + fp_templ.cols, monitor_fp_result.matchLoc.y + fp_templ.rows), cv::Scalar(0, 0, 255), 2, 8, 0);
	if (monitor_mp_result.minVal < monitor_hp_mp_fp_cutoffpoint) rectangle(img2, monitor_mp_result.matchLoc, cv::Point(monitor_mp_result.matchLoc.x + mp_templ.cols, monitor_mp_result.matchLoc.y + mp_templ.rows), cv::Scalar(0, 0, 255), 2, 8, 0);

	return img2;
}


cv::Mat debug_alert_text(flyff::window &win, const double x, const double y, const double width, const double height)
{


	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	RECT supportwindowrect_size{};
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	int windowxcord = static_cast<int>(supportwindowrect_size.right * x);
	int windowycord = static_cast<int>(supportwindowrect_size.bottom * y);
	int windowwidth = static_cast<int>(supportwindowrect_size.right * width);
	int windowheight = static_cast<int>(supportwindowrect_size.right * height);
	x_y_cords cords = check_x_y_cords(windowxcord, windowycord, windowwidth, windowheight, supportwindowrect_size);
	cv::Mat img = win.get_mat();
	cv::Mat img2 = img(cv::Rect(cords.windowxcord, cords.windowycord, cords.windowwidth, cords.windowheight));
	cv::cvtColor(img2, img2, cv::COLOR_BGRA2BGR);

	cv::Mat monster_taken_templ = cv::imread("images\\monstertaken.png");
	cv::cvtColor(monster_taken_templ, monster_taken_templ, cv::COLOR_BGRA2BGR);

	double monster_taken_cutoffpoint = 0.02;
	flyff::match_result monster_taken_result = match_flyff_item_alert_text(win, monster_taken_templ);
	std::cout << "The monster taken minVal is " << monster_taken_result.minVal << std::endl;

	if (monster_taken_result.minVal <= monster_taken_cutoffpoint) rectangle(img2, monster_taken_result.matchLoc, cv::Point(monster_taken_result.matchLoc.x + monster_taken_templ.cols, monster_taken_result.matchLoc.y + monster_taken_templ.rows), cv::Scalar(0, 0, 255), 2, 8, 0);

	return img2;
}


cv::Mat debug_combat(flyff::window &win, const double x, const double y, const double width, const double height)
{


	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	RECT supportwindowrect_size{};
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	int windowxcord = static_cast<int>(supportwindowrect_size.right * x);
	int windowycord = static_cast<int>(supportwindowrect_size.bottom * y);
	int windowwidth = static_cast<int>(supportwindowrect_size.right * width);
	int windowheight = static_cast<int>(supportwindowrect_size.right * height);
	x_y_cords cords = check_x_y_cords(windowxcord, windowycord, windowwidth, windowheight, supportwindowrect_size);
	cv::Mat	img = win.get_mat();
	cv::Mat img2 = img2(cv::Rect(cords.windowxcord, cords.windowycord, cords.windowwidth, cords.windowheight));
	cv::cvtColor(img2, img2, cv::COLOR_BGRA2BGR);


	cv::Mat combattempl = cv::imread("images\\combat.png");
	cv::cvtColor(combattempl, combattempl, cv::COLOR_BGRA2BGR);

	double combat_cutoffpoint = 0.009;
	flyff::match_result combat_result = match_specific_flyff_item(win, combattempl, flyff::type_of_match::combat);
	std::cout << "The combat minVal is " << combat_result.minVal << std::endl;

	if (combat_result.minVal <= combat_cutoffpoint) rectangle(img2, combat_result.matchLoc, cv::Point(combat_result.matchLoc.x + combattempl.cols, combat_result.matchLoc.y + combattempl.rows), cv::Scalar(0, 0, 255), 2, 8, 0);

	return img2;
}
