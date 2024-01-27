// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "game_window.h"

#include <iostream>


x_y_cords check_x_y_cords(int windowxcord, int windowycord, int windowwidth, int windowheight, RECT supportwindowrect_size)
{
	if (windowxcord < 0)
		windowxcord = 0;
	if (windowycord < 0)
		windowycord = 0;
	if (windowxcord + windowwidth > supportwindowrect_size.right)
		windowwidth = supportwindowrect_size.right - windowxcord;
	if (windowycord + windowheight > supportwindowrect_size.bottom)
		windowheight = supportwindowrect_size.bottom - windowycord;
	x_y_cords item{};
	item.windowxcord = windowxcord;
	item.windowycord = windowycord;
	item.windowwidth = windowwidth;
	item.windowheight = windowheight;
	item.supportwindowrect_size = supportwindowrect_size;
	return item;
}



cv::Mat flyff::window::get_mat(bool cursor)
{
	HWND supportwindow = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow)
	{
		supportwindow = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	if (!supportwindow)
	{
		std::cout << "Unable to find a Flyff Chrome Window \n"
		<< "Log in and press enter in this window to try again \n";
		std::cin.ignore();
		supportwindow = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
		if (!supportwindow)
		{
			supportwindow = FindWindowW(NULL, L"Flyff Universe - Chromium");
		}
		if (supportwindow)
		{
			std::cout << "Found the window, switch to the GUI to continue. \n";
		}
	}
	HDC hwindowDC, hwindowCompatibleDC;
	HBITMAP hbwindow;
	BITMAPINFOHEADER  bi;
	hwindowDC = GetDC(NULL);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);

	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);
	// needs error handling
	RECT supportwindowrect;
	GetWindowRect(supportwindow, &supportwindowrect);

	int h = supportwindowrect.bottom;
	int w = supportwindowrect.right;
	int height = h, width = w, srcheight = h, srcwidth = w;
	if (width < 0 && height < 0)
	{
		std::cout << "Unable to use the Flyff window, it should NOT be minimized \n"
		<< "Fix it and press enter to try again \n";
		std::cin.ignore();
		GetWindowRect(supportwindow, &supportwindowrect);

		h = supportwindowrect.bottom;
		w = supportwindowrect.right;
		height = h, width = w, srcheight = h, srcwidth = w;
	}
	window_.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !

	if (cursor == true)
	{
		// Draw the cursor
		CURSORINFO cinfo;
		cinfo.cbSize = sizeof(cinfo);
		GetCursorInfo(&cinfo);
		if (cinfo.hCursor && (cinfo.flags & CURSOR_SHOWING))
		{
			ICONINFO iinfo;
			if (GetIconInfo((HICON)cinfo.hCursor, &iinfo))
			{
				int x = cinfo.ptScreenPos.x - iinfo.xHotspot;
				int y = cinfo.ptScreenPos.y - iinfo.yHotspot;
				HICON hIcon = CopyIcon((HICON)cinfo.hCursor);
				DrawIcon(hwindowCompatibleDC, x, y, hIcon);
				DestroyIcon(hIcon);
				DeleteObject(iinfo.hbmMask);
				DeleteObject(iinfo.hbmColor);
			}
		}
	}

	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, window_.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(supportwindow, hwindowDC);
	return window_;
}


cv::Mat flyff::window::get_and_convert_mat(cv::ColorConversionCodes color)
{
	flyff::window::get_mat();
	cv::cvtColor(window_, window_, color);
	return window_;
}


void flyff::window::free()
{
	window_.release();
}


Flyff_screen_cords get_flyff_char_screen_cords()
{
	RECT supportwindowrect_size{};
	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	int end_xcord = static_cast<int>(supportwindowrect_size.right * 0.2);
	int end_ycord = static_cast<int>(supportwindowrect_size.bottom * 0.26);
	Flyff_screen_cords cords{};
	cords.start_x = 0;
	cords.start_y = 0;
	cords.end_x = end_xcord;
	cords.end_y = end_ycord;
	return cords;
}

Flyff_screen_cords get_flyff_map_cords()
{
	RECT supportwindowrect_size{};
	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	int xoffset = static_cast<int>(supportwindowrect_size.right * 0.2);
	int start_xcord = static_cast<int>(supportwindowrect_size.right - xoffset);
	int end_xcord = supportwindowrect_size.right;
	int end_ycord = static_cast<int>(supportwindowrect_size.bottom * 0.3);

	Flyff_screen_cords cords{};
	cords.start_x = start_xcord;
	cords.start_y = 0;
	cords.end_x = end_xcord;
	cords.end_y = end_ycord;
	return cords;
}

int get_bottom_of_screen_y_cord()
{
	RECT supportwindowrect_size;
	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	int windowycord = static_cast<int>(supportwindowrect_size.bottom * 0.9);
	return windowycord;
}


center_window_data flyff::window::get_mat_from_center_of_window(const double x, const double y, const double w, const double h)
{
	cv::Mat img, img2;
	RECT supportwindowrect_size;
	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	int windowxcord = static_cast<int>(supportwindowrect_size.right * x);
	int windowycord = static_cast<int>(supportwindowrect_size.bottom * y);
	int windowwidth = static_cast<int>(supportwindowrect_size.right * w);
	int windowheight = static_cast<int>(supportwindowrect_size.bottom * h);
	x_y_cords cords = check_x_y_cords(windowxcord, windowycord, windowwidth, windowheight, supportwindowrect_size);
	center_window_data_.win_xcord = cords.windowxcord;
	center_window_data_.win_ycord = cords.windowycord;
	img = window_;
	img2 = img(cv::Rect(cords.windowxcord, cords.windowycord, cords.windowwidth, cords.windowheight));
	cv::cvtColor(img2, img2, cv::COLOR_BGRA2BGR);
	center_window_data_.img = img2;
	return center_window_data_;
}
