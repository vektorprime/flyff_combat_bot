// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "image_matching.h"

#include "mouse_and_keyboard.h"


namespace flyff
{
	match_result match_flyff_item(window &win, cv::Mat templ, cv::ColorConversionCodes color)
	{
		cv::Mat result;
		double minVal = 0.0; double maxVal = 0.0; cv::Point minLoc; cv::Point maxLoc;
		cv::Mat originalimage = win.get_and_convert_mat(color);
		int result_cols = originalimage.cols - templ.cols + 1;
		int result_rows = originalimage.rows - templ.rows + 1;
		result.create(result_rows, result_cols, CV_8UC4);
		matchTemplate(originalimage, templ, result, cv::TM_SQDIFF_NORMED);
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
		match_result item;
		item.minVal = minVal;
		item.minLoc = minLoc;
		item.maxLoc = maxLoc;
		item.result_cols = result_cols;
		item.result_rows = result_rows;
		item.matchLoc = minLoc;
		item.templ = templ;
		return item;
	}


	//at some point i'll change this to be an overloaded function instead of a switch statement embedded inside
	match_result match_specific_flyff_item(window &win, cv::Mat templ, const type_of_match type_of_match)
	{
		RECT supportwindowrect_size;
		HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
		if (!supportwindow_size)
		{
			supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
		}
		GetWindowRect(supportwindow_size, &supportwindowrect_size);
		int windowxcord = 0;
		int windowycord = 0;
		int windowwidth = supportwindowrect_size.right;
		int windowheight = supportwindowrect_size.bottom;

		switch (type_of_match)
		{
		case type_of_match::char_screen:
			windowxcord = 0;
			windowycord = static_cast<int>(supportwindowrect_size.bottom * 0.085);
			windowwidth = static_cast<int>(supportwindowrect_size.right * 0.25);
			windowheight = static_cast<int>(supportwindowrect_size.bottom * 0.2);
			break;
		case type_of_match::monster_target:
			windowxcord = static_cast<int>(supportwindowrect_size.right * 0.3);
			windowycord = static_cast<int>(supportwindowrect_size.bottom * 0.1);
			windowwidth = static_cast<int>(supportwindowrect_size.right * 0.3);
			windowheight = static_cast<int>(supportwindowrect_size.bottom * 0.25);
			break;
		case type_of_match::combat_monster:
			windowxcord = static_cast<int>(supportwindowrect_size.right * 0.3);
			windowycord = static_cast<int>(supportwindowrect_size.bottom * 0.3);
			windowwidth = static_cast<int>(supportwindowrect_size.right * 0.3);
			windowheight = static_cast<int>(supportwindowrect_size.bottom * 0.3);
			break;
		case type_of_match::party_window:
			windowxcord = 0;
			windowycord = static_cast<int>(supportwindowrect_size.bottom * 0.9);
			windowwidth = static_cast<int>(supportwindowrect_size.right * 0.2);
			windowheight = static_cast<int>(supportwindowrect_size.bottom * 0.1);
			break;
		case type_of_match::combat:
			windowxcord = static_cast<int>(supportwindowrect_size.right * 0.8);
			windowycord = static_cast<int>(supportwindowrect_size.bottom * 0.25);
			windowwidth = static_cast<int>(supportwindowrect_size.right * 0.2);
			windowheight = static_cast<int>(supportwindowrect_size.bottom * 0.1);
			break;
		case type_of_match::skillbar:
			windowxcord = static_cast<int>(supportwindowrect_size.right * 0.1);
			windowycord = static_cast<int>(supportwindowrect_size.bottom * 0.9);
			windowwidth = static_cast<int>(supportwindowrect_size.right * 0.8);
			windowheight = static_cast<int>(supportwindowrect_size.bottom * 0.1);
			break;
		case type_of_match::alert_text:
			windowxcord = static_cast<int>(supportwindowrect_size.right * 0.3);
			windowycord = static_cast<int>(supportwindowrect_size.bottom * 0.5);
			windowwidth = static_cast<int>(supportwindowrect_size.right * 0.3);
			windowheight = static_cast<int>(supportwindowrect_size.bottom * 0.3);
			break;
		case type_of_match::navigator_icon:
			windowxcord = static_cast<int>(supportwindowrect_size.right * 0.8);
			windowycord = 0;
			windowwidth = static_cast<int>(supportwindowrect_size.right * 0.2);
			windowheight = static_cast<int>(supportwindowrect_size.bottom * 0.35);
			break;
		}

		//check cords to to make sure we're not out of bounds
		x_y_cords item_cords = check_x_y_cords(windowxcord, windowycord, windowwidth, windowheight, supportwindowrect_size);
		cv::Mat img1 = win.get_mat();
		cv::Mat img2 = img1(cv::Rect(item_cords.windowxcord, item_cords.windowycord, item_cords.windowwidth, item_cords.windowheight));
		cv::cvtColor(img2, img2, cv::COLOR_BGRA2BGR);
		int result_cols = img2.cols - templ.cols + 1;
		int result_rows = img2.rows - templ.rows + 1;
		cv::Mat result;
		result.create(result_rows, result_cols, CV_8UC4);
		matchTemplate(img2, templ, result, cv::TM_SQDIFF_NORMED);
		double minVal = 0.0; double maxVal = 0.0; cv::Point minLoc; cv::Point maxLoc;
		cv::Point matchLoc;
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
		matchLoc = minLoc;
		match_result item;
		item.minVal = minVal;
		// this gives the real cord
		item.minLoc = cv::Point(minLoc.x + windowxcord, minLoc.y + windowycord);
		item.maxLoc = maxLoc;
		item.result_cols = result_cols;
		item.result_rows = result_rows;
		// this gives the real cord toozz
		item.matchLoc = cv::Point(matchLoc.x + windowxcord, matchLoc.y + windowycord);
		item.templ = templ;
		////debug test
		//bool debug_heal = false;
		//if (debug_heal == true && flyff::type_of_match == flyff::type_of_match::skillbar)
		//{
		//	if (item.minVal < 0.003) { rectangle(img2, matchLoc, cv::Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), cv::Scalar(0, 0, 255), 1, 8, 0); }
		//	cv::imshow("heal skill", img2);
		//	cv::waitKey(1);
		//}
		////end debug test
		return item;
	}



	match_result match_flyff_item_alert_text(window &win, cv::Mat templ)
	{
		cv::Mat img, img2, result;
		RECT supportwindowrect_size;
		HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
		if (!supportwindow_size)
		{
			supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
		}
		GetWindowRect(supportwindow_size, &supportwindowrect_size);
		int windowxcord = static_cast<int>(supportwindowrect_size.right * 0.3);
		int windowycord = static_cast<int>(supportwindowrect_size.bottom * 0.5);
		int windowwidth = static_cast<int>(supportwindowrect_size.right * 0.3);
		int windowheight = static_cast<int>(supportwindowrect_size.bottom * 0.3);
		x_y_cords cords = check_x_y_cords(windowxcord, windowycord, windowwidth, windowheight, supportwindowrect_size);
		img = win.get_mat();
		img2 = img(cv::Rect(cords.windowxcord, cords.windowycord, cords.windowwidth, cords.windowheight));
		cv::cvtColor(img2, img2, cv::COLOR_BGRA2BGR);
		int result_cols = img2.cols - templ.cols + 1;
		int result_rows = img2.rows - templ.rows + 1;
		result.create(result_rows, result_cols, CV_8UC4);
		matchTemplate(img, templ, result, cv::TM_SQDIFF_NORMED);
		double minVal = 0.0; double maxVal = 0.0; cv::Point minLoc; cv::Point maxLoc;
		cv::Point matchLoc;
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
		matchLoc = minLoc;
		match_result item;
		item.minVal = minVal;
		// this gives the real cord
		item.minLoc = cv::Point(minLoc.x + windowxcord, minLoc.y + windowycord);
		item.maxLoc = maxLoc;
		item.result_cols = result_cols;
		item.result_rows = result_rows;
		// this gives the real cord too
		item.matchLoc = cv::Point(matchLoc.x + windowxcord, matchLoc.y + windowycord);
		item.templ = templ;
		return item;
	}


	match_result match_flyff_item_with_cursor(window &win, cv::Mat templ)
	{
		cv::Mat result, img;
		double minVal = 0.0; double maxVal = 0.0; cv::Point minLoc; cv::Point maxLoc; cv::Point matchLoc;
		img = win.get_mat(true);
		cv::cvtColor(img, img, cv::COLOR_BGRA2BGR);
		int result_cols = img.cols - templ.cols + 1;
		int result_rows = img.rows - templ.rows + 1;
		result.create(result_rows, result_cols, CV_8UC4);
		matchTemplate(img, templ, result, cv::TM_SQDIFF_NORMED);
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
		matchLoc = minLoc;
		match_result item;
		item.minVal = minVal;
		item.minLoc = minLoc;
		item.maxLoc = maxLoc;
		item.result_cols = result_cols;
		item.result_rows = result_rows;
		item.matchLoc = matchLoc;
		item.templ = templ;
		return item;
	}


	match_result match_flyff_item_map_zones(window &win, cv::Mat templ, const int xoffset, const int yoffset)
	{
		//offset for top left of map is 0,0.
		//offset for top right of map is 30, 0
		//offset for bottom left of map is 0, 30
		//offset for bottom right of map is 30, 30
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
		double minVal = 0.0; double maxVal = 0.0; cv::Point minLoc; cv::Point maxLoc;
		cv::Point matchLoc;
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
		matchLoc = minLoc;
		match_result item;
		item.minVal = minVal;
		item.minLoc = minLoc;
		item.maxLoc = maxLoc;
		item.result_cols = result_cols;
		item.result_rows = result_rows;
		item.matchLoc = matchLoc;
		item.templ = templ;
		return item;
	}


	match_result match_flyff_item_combat_monster_focused(window &win, cv::Mat templ, int const last_xloc, const int last_yloc)
	{
		cv::Mat img, img2, result;
		RECT supportwindowrect_size;
		HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
		if (!supportwindow_size)
		{
			supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
		}
		GetWindowRect(supportwindow_size, &supportwindowrect_size);
		int windowxcord = last_xloc;
		int windowycord = last_yloc;
		int windowwidth = 200;
		int windowheight = 200;
		x_y_cords cords = check_x_y_cords(windowxcord, windowycord, windowwidth, windowheight, supportwindowrect_size);
		img = win.get_mat();
		img2 = img(cv::Rect(cords.windowxcord, cords.windowycord, cords.windowwidth, cords.windowheight));
		cv::cvtColor(img2, img2, cv::COLOR_BGRA2BGR);
		int result_cols = img2.cols - templ.cols + 1;
		int result_rows = img2.rows - templ.rows + 1;
		result.create(result_rows, result_cols, CV_8UC4);
		matchTemplate(img2, templ, result, cv::TM_SQDIFF_NORMED);
		double minVal = 0.0; double maxVal = 0.0; cv::Point minLoc; cv::Point maxLoc;
		cv::Point matchLoc;
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
		matchLoc = minLoc;
		flyff::match_result item;
		item.minVal = minVal;
		// this gives the real cord
		item.minLoc = cv::Point(minLoc.x + windowxcord, minLoc.y + windowycord);
		item.maxLoc = maxLoc;
		item.result_cols = result_cols;
		item.result_rows = result_rows;
		// this gives the real cord too
		item.matchLoc = cv::Point(matchLoc.x + windowxcord, matchLoc.y + windowycord);
		item.templ = templ;
		return item;
	}

}
//end namespace


bool find_target_icons(flyff::window &win)
{
	//This function is used by the healer loop to make sure that the 
	//target (fighter) is always near, if it's not, other code will execute to find and follow

	float target_cutoffpoint{ 0.001 };

	cv::Mat target_bl_templ = cv::imread("images\\target_bl.png");
	cv::cvtColor(target_bl_templ, target_bl_templ, cv::COLOR_BGRA2BGR);
	flyff::match_result target_bl_result = flyff::match_flyff_item(win, target_bl_templ);

	cv::Mat target_br_templ = cv::imread("images\\target_br.png");
	cv::cvtColor(target_br_templ, target_br_templ, cv::COLOR_BGRA2GRAY);
	flyff::match_result target_br_result = flyff::match_flyff_item(win, target_br_templ, cv::COLOR_BGRA2GRAY);

	cv::Mat target_tl_templ = cv::imread("images\\target_tl.png");
	cv::cvtColor(target_tl_templ, target_tl_templ, cv::COLOR_BGRA2GRAY);
	flyff::match_result target_tl_result = flyff::match_flyff_item(win, target_tl_templ, cv::COLOR_BGRA2GRAY);

	cv::Mat target_tr_templ = cv::imread("images\\target_tr.png");
	cv::cvtColor(target_tr_templ, target_tr_templ, cv::COLOR_BGRA2GRAY);
	flyff::match_result target_tr_result = flyff::match_flyff_item(win, target_tr_templ, cv::COLOR_BGRA2GRAY);

	bool result = false;

	if (target_bl_result.minVal < target_cutoffpoint ||
		target_br_result.minVal < target_cutoffpoint ||
		target_tl_result.minVal < target_cutoffpoint ||
		target_tr_result.minVal < target_cutoffpoint )
	{
		result = true;
	}
	return result;
}


screen_cords get_target_to_click_cords(flyff::window &win)
{
	//This function will be used in conjunction with the memory readers to defend the healer or target monsters via memory and click.
	//We can't directly attack from memory, we need an emulated click first.
	//need to match 4 of the target icons so that we can get the center position.

	screen_cords target;
	
	cv::Mat target_bl_templ = cv::imread("images\\target_bl.png");
	cv::cvtColor(target_bl_templ, target_bl_templ, cv::COLOR_BGRA2GRAY);
	flyff::match_result target_bl_result = flyff::match_flyff_item(win, target_bl_templ, cv::COLOR_BGRA2GRAY);

	cv::Mat target_br_templ = cv::imread("images\\target_br.png");
	cv::cvtColor(target_br_templ, target_br_templ, cv::COLOR_BGRA2GRAY);
	flyff::match_result target_br_result = flyff::match_flyff_item(win, target_br_templ, cv::COLOR_BGRA2GRAY);

	cv::Mat target_tl_templ = cv::imread("images\\target_tl.png");
	cv::cvtColor(target_tl_templ, target_tl_templ, cv::COLOR_BGRA2GRAY);
	flyff::match_result target_tl_result = flyff::match_flyff_item(win, target_tl_templ, cv::COLOR_BGRA2GRAY);
	
	cv::Mat target_tr_templ = cv::imread("images\\target_tr.png");
	cv::cvtColor(target_tr_templ, target_tr_templ, cv::COLOR_BGRA2GRAY);
	flyff::match_result target_tr_result = flyff::match_flyff_item(win, target_tr_templ, cv::COLOR_BGRA2GRAY);
	
	float target_cutoffpoint{ 0.001 };

	//we need multiple fall backs to getting the target to click, we can still calculate the coords with missing missing targets
	if (target_bl_result.minVal <= target_cutoffpoint &&
		target_tl_result.minVal <= target_cutoffpoint &&
		target_tr_result.minVal <= target_cutoffpoint)
	{
		//this omits the bottom right
		//if we made it this far then we have a valid match for 3 of the target icons and we can use our cords later on
		target.x = target_tl_result.minLoc.x + ((target_tr_result.minLoc.x - target_tl_result.minLoc.x) / 2);
		target.y = target_tl_result.minLoc.y + ((target_bl_result.minLoc.y - target_tl_result.minLoc.y) / 2);

		return target;
	}

	if (target_br_result.minVal <= target_cutoffpoint &&
		target_tl_result.minVal <= target_cutoffpoint &&
		target_tr_result.minVal <= target_cutoffpoint)
	{
		//this omits the bottom left
		//if we made it this far then we have a valid match for 3 of the target icons and we can use our cords later on
		target.x = target_tl_result.minLoc.x + ((target_tr_result.minLoc.x - target_tl_result.minLoc.x) / 2);
		target.y = target_tl_result.minLoc.y + ((target_br_result.minLoc.y - target_tl_result.minLoc.y) / 2);

		return target;
	}

	if (target_bl_result.minVal <= target_cutoffpoint &&
		target_br_result.minVal <= target_cutoffpoint)

	{
		//this omits the top left and right
		//if we made it this far then we have a valid match for 2 of the target icons and we need to guess the rest
		target.x = target_bl_result.minLoc.x + ((target_br_result.minLoc.x - target_bl_result.minLoc.x) / 2);
		target.y = target_bl_result.minLoc.y - ((target_br_result.minLoc.x - target_bl_result.minLoc.x) / 1.5);

		return target;
	}

	if (target_tl_result.minVal <= target_cutoffpoint &&
		target_tr_result.minVal <= target_cutoffpoint)
	{
		//this omits the bottom left and right
		//if we made it this far then we have a valid match for 2 of the target icons and we need to guess the rest
		target.x = target_tl_result.minLoc.x + ((target_tr_result.minLoc.x - target_tl_result.minLoc.x) / 2);
		target.y = target_tl_result.minLoc.y + ((target_tr_result.minLoc.x - target_tl_result.minLoc.x) / 2);

		return target;
	}

	return target;

}



void exit_monster_target(flyff::window &win, cv::Mat monster_target_templ)
{
	flyff::match_result monster_target_result = flyff::match_specific_flyff_item(win, monster_target_templ, flyff::type_of_match::monster_target);
	if (monster_target_result.minVal < 0.09)
	{
		send_key_hold_down(VK_ESCAPE);
	}
}




int find_nav_angle(flyff::window &win)
{
	cv::Mat img, originalimage, originalimage2;
	// Load the map area
	RECT supportwindowrect_size;
	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	originalimage = win.get_mat();
	int windowxcord = static_cast<int>(supportwindowrect_size.right - 120);
	int windowycord = 170;
	int windowwidth = 50;
	int windowheight = 50;
	x_y_cords cords = check_x_y_cords(windowxcord, windowycord, windowwidth, windowheight, supportwindowrect_size);
	originalimage2 = originalimage(cv::Rect(cords.windowxcord, cords.windowycord, cords.windowwidth, cords.windowheight));
	cv::cvtColor(originalimage2, img, cv::COLOR_BGRA2BGR);

	cv::Mat sourceImage;
	cv::cvtColor(img, sourceImage, cv::COLOR_BGRA2BGR);

	// Load the source image and the template image
	cv::Mat templateImageunchanged = cv::imread("images\\compassfull.png", cv::IMREAD_COLOR);
	cv::Mat templateImage;
	cv::cvtColor(templateImageunchanged, templateImage, cv::COLOR_BGRA2BGR);
	// Determine the desired vertices of the triangle within the square image
	cv::Point vertex1(8, 4);
	cv::Point vertex2(10, 16);
	cv::Point vertex3(5, 16);

	// Create a blank mask with the same dimensions as the template image
	cv::Mat mask = cv::Mat::zeros(templateImage.size(), CV_8UC1);
	cv::Mat mask2 = cv::Mat::zeros(templateImage.size(), CV_8UC1);
	// Draw a filled triangle on the mask image
	cv::fillConvexPoly(mask, std::vector<cv::Point>{vertex1, vertex2, vertex3}, cv::Scalar(255));
	// Apply the mask to the template image
	cv::Mat croppedImage;
	cv::bitwise_and(templateImage, templateImage, croppedImage, mask);

	// Define the range of rotation angles (in degrees)
	int startAngle = 0;
	int endAngle = 360;
	int angleStep = 1;

	cv::Point bestMatchLoc;
	double bestMatchValue = 1;
	double bestMatchAngle = 0;

	// Iterate over each rotation angle
	for (int angle = startAngle; angle <= endAngle; angle += angleStep)
	{
		// Rotate the template image
		cv::Mat rotatedTemplate;
		cv::Point2f center(static_cast<float>(croppedImage.cols / 2), static_cast<float>(croppedImage.rows / 2));
		cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
		cv::warpAffine(croppedImage, rotatedTemplate, rotationMatrix, croppedImage.size());
		// Perform template matching
		cv::Mat result;
		cv::matchTemplate(sourceImage, rotatedTemplate, result, cv::TM_CCORR);
		// Find the best match
		double minVal, maxVal;
		cv::Point minLoc, maxLoc;
		cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
		// Update the best match
		if (maxVal > bestMatchValue)
		{
			// std::cout << "maxVal is " << maxVal << std::endl;
			bestMatchValue = maxVal;
			bestMatchLoc = maxLoc;
			bestMatchAngle = angle;
			// std::cout << "The current angle is " << angle << std::endl;
		}
	}

	return bestMatchAngle;
}




bool check_if_disconnected(flyff::window &win, const disconnect_message &message, flyff::time &disconnected_time, flyff::match_result &disconnected_result)
{
	double disconnected_cutoffpoint = 0.0003;

	//std::cout << " Assuming refreshbutton.png is in the current folder. \n" << std::endl;
	cv::Mat refresh_button_templ = cv::imread("images\\refreshbutton.png");
	cv::cvtColor(refresh_button_templ, refresh_button_templ, cv::COLOR_BGRA2BGR);
	//std::cout << " Assuming disconnected.png is in the current folder. \n" << std::endl;
	std::string template_image;
	switch (message)
	{
		case disconnect_message::disconnected:
			template_image = "images\\disconnected.png";
			break;
		case disconnect_message::cannot_connect:
			template_image = "images\\cannotconnect.png";
			break;
		case disconnect_message::account_connected:
			template_image = "images\\accountconnected.png";
			break;
		case disconnect_message::connecting:
			template_image = "images\\connecting.png";
			break;
	}


	cv::Mat disconnected_templ = cv::imread(template_image);
	cv::cvtColor(disconnected_templ, disconnected_templ, cv::COLOR_BGRA2BGR);

	if (disconnected_time.check_if_time_elapsed())
	{
		disconnected_result = match_flyff_item(win, disconnected_templ);
		std::cout << "checking disconnected_result, the minVal is " << disconnected_result.minVal << std::endl;
		if (disconnected_result.minVal < disconnected_cutoffpoint)
		{
			std::cout << "found disconnected message, sleeping for 30 sec" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(30));
			std::cout << "Looking for refresh button on browser and clicking it" << std::endl;

			flyff::match_result refresh_button_result = match_flyff_item(win, refresh_button_templ);

			send_left_click_delay(refresh_button_result.minLoc.x + 10, refresh_button_result.minLoc.y + 10);

			std::this_thread::sleep_for(std::chrono::seconds(10));

			disconnected_time.set_start_time();

			return true;
		}
		//regardless of finding disconnected message or not, only check every 30 sec
		disconnected_time.set_start_time();
	}
	return false;
}


void check_if_char_screen_icon_missing(flyff::window &win, flyff::time &char_scree_icon_time, flyff::log &log_file)
{
	if (char_scree_icon_time.check_if_time_elapsed())
	{
		if (log_file.log_level_is_verbose())
		{
			log_file.write("Starting check_if_char_screen_icon_missing");
		}
		const float char_screen_icon_cutoffpoint = 0.003;
		cv::Mat char_screen_icon_templ = cv::imread("images\\char_screen_icon.png");
		cv::cvtColor(char_screen_icon_templ, char_screen_icon_templ, cv::COLOR_BGRA2BGR);
		flyff::match_result char_screen_icon_result = flyff::match_specific_flyff_item(win, char_screen_icon_templ, flyff::type_of_match::char_screen);
		if (char_screen_icon_result.minVal > char_screen_icon_cutoffpoint)
		{
			if (log_file.log_level_is_verbose())
			{
				log_file.write("Unable to find char screen, sending T key");
			}
			std::cout << "Unable to find char screen icon, sending T key" << std::endl;
			send_key_hold_down(0x54);
		}
		char_scree_icon_time.set_start_time();
	}
}
