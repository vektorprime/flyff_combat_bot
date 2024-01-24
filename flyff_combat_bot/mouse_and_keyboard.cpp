// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "mouse_and_keyboard.h"
#include "memory_reader.h"

#include <iostream>
#include <string> 

// Originally I was only using send_key_hold_down, but I started to experience strange, undefined behavior with C++ optimizations on. I need tom come back and troubleshoot it. For now I will use some separate functions.

void send_key_hold_down(char numpress, int time_in_millisec)
{
	// Check if the key is already pressed
	if (GetAsyncKeyState(numpress) & 0x8000)
	{
		// Key is already down; no need to send another key-down event
	}
	else
	{
		// Key is not down; send a key-down event
		INPUT key_down = {};
		key_down.type = INPUT_KEYBOARD;
		key_down.ki.wVk = numpress;
		SendInput(1, &key_down, sizeof(INPUT));
	}

	// Pause the thread for TIME milliseconds
	std::this_thread::sleep_for(std::chrono::milliseconds(time_in_millisec));

	// Send the key-up event
	INPUT key_up = {};
	key_up.type = INPUT_KEYBOARD;
	key_up.ki.wVk = numpress;
	key_up.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &key_up, sizeof(INPUT));

}

void send_key_down(char key)
{
	// Check if the key is already pressed
	if (GetAsyncKeyState(key) & 0x8000)
	{
		// Key is already down; no need to send another key-down event
	}
	else
	{
		// Key is not down; send a key-down event
		INPUT key_down = {};
		key_down.type = INPUT_KEYBOARD;
		key_down.ki.wVk = key;
		SendInput(1, &key_down, sizeof(INPUT));
	}
}

void send_key_up(char key)
{
	INPUT key_up = {};
	key_up.type = INPUT_KEYBOARD;
	key_up.ki.wVk = key;
	key_up.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &key_up, sizeof(INPUT));
}

void send_2_keys_and_hold_first(char key1, int time_in_millisec, char key2)
{

	send_key_down(key1);
	std::this_thread::sleep_for(std::chrono::milliseconds(time_in_millisec));
	send_key_hold_down(key2, time_in_millisec);
	send_key_up(key1);
}


//#pragma optimize("", on)


int send_jump_and_run()
{
	// Check if both keys are already pressed
	if ((GetAsyncKeyState(0x57) & 0x8000) && (GetAsyncKeyState(VK_SPACE) & 0x8000))
	{
		// Both keys are already down; no need to send another key-down event
	}
	else
	{
		// Create a vector/array of key strokes to press
		std::vector<INPUT> myinputs;

		// Key down event for "W" key
		INPUT key_down_w;
		key_down_w.type = INPUT_KEYBOARD;
		key_down_w.ki.wVk = 0x57; // "W" key
		key_down_w.ki.dwFlags = 0;
		myinputs.push_back(std::move(key_down_w));

		// Key down event for space bar
		INPUT key_down_space;
		key_down_space.type = INPUT_KEYBOARD;
		key_down_space.ki.wVk = VK_SPACE; // Space bar
		key_down_space.ki.dwFlags = 0;
		myinputs.push_back(std::move(key_down_space));

		// Send both key-down events
		SendInput(static_cast<UINT>(myinputs.size()), myinputs.data(), sizeof(INPUT));
	}

	// Pause the thread for a duration (e.g., 250 milliseconds)
	std::this_thread::sleep_for(std::chrono::milliseconds(250));

	// Create a vector for key-up events
	std::vector<INPUT> myupinputs;

	// Key up event for "W" key
	INPUT key_up_w;
	key_up_w.type = INPUT_KEYBOARD;
	key_up_w.ki.wVk = 0x57; // "W" key
	key_up_w.ki.dwFlags = KEYEVENTF_KEYUP;
	myupinputs.push_back(std::move(key_up_w));

	// Key up event for space bar
	INPUT key_up_space;
	key_up_space.type = INPUT_KEYBOARD;
	key_up_space.ki.wVk = VK_SPACE; // Space bar
	key_up_space.ki.dwFlags = KEYEVENTF_KEYUP;
	myupinputs.push_back(std::move(key_up_space));

	// Send both key-up events
	SendInput(static_cast<UINT>(myupinputs.size()), myupinputs.data(), sizeof(INPUT));

	return 0;
}

int send_jump()
{
	// Check if both keys are already pressed
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		// Both keys are already down; no need to send another key-down event
	}
	else
	{
		// Create a vector/array of key strokes to press
		std::vector<INPUT> myinputs;


		// Key down event for space bar
		INPUT key_down_space;
		key_down_space.type = INPUT_KEYBOARD;
		key_down_space.ki.wVk = VK_SPACE; // Space bar
		key_down_space.ki.dwFlags = 0;
		myinputs.push_back(std::move(key_down_space));

		// Send both key-down events
		SendInput(static_cast<UINT>(myinputs.size()), myinputs.data(), sizeof(INPUT));
	}

	// Pause the thread for a duration (e.g., 250 milliseconds)
	std::this_thread::sleep_for(std::chrono::milliseconds(250));

	// Create a vector for key-up events
	std::vector<INPUT> myupinputs;

	// Key up event for space bar
	INPUT key_up_space;
	key_up_space.type = INPUT_KEYBOARD;
	key_up_space.ki.wVk = VK_SPACE; // Space bar
	key_up_space.ki.dwFlags = KEYEVENTF_KEYUP;
	myupinputs.push_back(std::move(key_up_space));

	// Send both key-up events
	SendInput(static_cast<UINT>(myupinputs.size()), myupinputs.data(), sizeof(INPUT));

	return 0;
}



int send_rightarrow_down()
{
	//hold the right arrow for 1 sec down
	// create a vector/array of key strokes to press
	std::vector<INPUT> myinputs;
	// press down right arrow and add to array
	INPUT rightarrow_down;
	rightarrow_down.type = INPUT_KEYBOARD;
	rightarrow_down.ki.wVk = VK_RIGHT;
	rightarrow_down.ki.dwFlags = 0;
	myinputs.push_back(std::move(rightarrow_down));

	// send the key presses
	// first parameter needs the number of structs in array, second param is an array of INPUT, third param is the size of an INPUT struct.
	SendInput(static_cast<UINT>(myinputs.size()), myinputs.data(), sizeof(INPUT));
	return 0;
}

int send_rightarrow_up()
{
	//hold the right arrow for 1 sec down
	// create a vector/array of key strokes to press
	std::vector<INPUT> myinputs;

	// release up arrow and add to array
	INPUT rightarrow_up;
	rightarrow_up.type = INPUT_KEYBOARD;
	rightarrow_up.ki.wVk = VK_RIGHT;
	rightarrow_up.ki.dwFlags = KEYEVENTF_KEYUP;
	myinputs.push_back(std::move(rightarrow_up));
	// send the key presses
	// first parameter needs the number of structs in array, second param is an array of INPUT, third param is the size of an INPUT struct.
	SendInput(static_cast<UINT>(myinputs.size()), myinputs.data(), sizeof(INPUT));
	return 0;
}


int send_left_click(const int mousexloc, const int mouseyloc)
{
	int screenwidth = GetSystemMetrics(SM_CXFULLSCREEN);
	int screenheight = GetSystemMetrics(SM_CYFULLSCREEN);

	// create a vector/array of key strokes to press
	std::vector<INPUT> myinputs;

	// move mouse to x and y
	INPUT move_mouse;
	ZeroMemory(&move_mouse, sizeof(move_mouse));
	move_mouse.type = INPUT_MOUSE;
	move_mouse.mi.time = 0;
	move_mouse.mi.mouseData = 0;
	move_mouse.mi.dwExtraInfo = 0;
	move_mouse.mi.dx = 65536 / screenwidth * mousexloc;
	move_mouse.mi.dy = 65536 / screenheight * mouseyloc;
	move_mouse.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	myinputs.push_back(std::move(move_mouse));

	// press down on left click
	INPUT click_down;
	ZeroMemory(&click_down, sizeof(click_down));
	click_down.type = INPUT_MOUSE;
	click_down.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	myinputs.push_back(std::move(click_down));

	// release the left click
	INPUT click_up;
	ZeroMemory(&click_up, sizeof(click_up));
	click_up.type = INPUT_MOUSE;
	click_up.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	myinputs.push_back(std::move(click_up));

	// send the key presses
	// first parameter needs the number of structs in array, second param is an array of INPUT, third param is the size of an INPUT struct.
	SendInput(static_cast<UINT>(myinputs.size()), myinputs.data(), sizeof(INPUT));
	return 0;
}


int send_left_click_delay(const int mousexloc, const int mouseyloc)
{
	int screenwidth = GetSystemMetrics(SM_CXSCREEN);
	int screenheight = GetSystemMetrics(SM_CYSCREEN);

	// move mouse to x and y
	INPUT move_mouse;
	ZeroMemory(&move_mouse, sizeof(move_mouse));
	move_mouse.type = INPUT_MOUSE;
	move_mouse.mi.dx = (65536 / screenwidth) * mousexloc;
	move_mouse.mi.dy = (65536 / screenheight) * mouseyloc;
	move_mouse.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	SendInput(1, &move_mouse, sizeof(INPUT));

	std::this_thread::sleep_for(std::chrono::milliseconds(250));

	// press down on left click
	INPUT click_down = move_mouse;
	click_down.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &click_down, sizeof(INPUT));

	// release the left click
	INPUT click_up = move_mouse;
	click_up.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP;
	SendInput(1, &click_up, sizeof(INPUT));

	return 0;
}

int move_mouse(const int mousexloc, const int mouseyloc)
{
	int screenwidth = GetSystemMetrics(SM_CXSCREEN);
	int screenheight = GetSystemMetrics(SM_CYSCREEN);

	std::vector<INPUT> myinputs;
	INPUT move_mouse;
	ZeroMemory(&move_mouse, sizeof(move_mouse));
	move_mouse.type = INPUT_MOUSE;
	move_mouse.mi.time = 0;
	move_mouse.mi.mouseData = 0;
	move_mouse.mi.dwExtraInfo = 0;
	move_mouse.mi.dx = 65536 / screenwidth * mousexloc;
	move_mouse.mi.dy = 65536 / screenheight * mouseyloc;
	move_mouse.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	myinputs.push_back(std::move(move_mouse));
	SendInput(static_cast<UINT>(myinputs.size()), myinputs.data(), sizeof(INPUT));
	return 0;
}

int send_right_arrow()
{
	auto now = std::chrono::steady_clock::now;
	std::chrono::steady_clock::duration work_duration = 100ms;
	auto start = now();

	while ((now() - start) < work_duration)
	{
		send_rightarrow_down();
	}
	send_rightarrow_up();
	return 0;
}

int send_buffs()
{
	//follow with Z
	send_key_hold_down(0x5A, 500);
	//switch to first skill bar
	send_key_hold_down(VK_F1, 500);
	//send a heal every 2 buffs
	send_key_hold_down(0x31, 2000);
	send_key_hold_down(0x32, 2000);
	send_key_hold_down(0x33, 2000);
	send_key_hold_down(0x31, 2000);
	send_key_hold_down(0x34, 2000);
	send_key_hold_down(0x35, 2000);
	send_key_hold_down(0x31, 2000);
	send_key_hold_down(0x36, 2000);
	send_key_hold_down(0x37, 2000);
	send_key_hold_down(0x31, 2000);
	send_key_hold_down(0x38, 2000);
	send_key_hold_down(0x39, 2000);
	send_key_hold_down(0x31, 2000);
	send_key_hold_down(0x30, 2000);
	send_key_hold_down(VK_F2, 500);
	send_key_hold_down(0x32, 2000);
	send_key_hold_down(0x31, 2000);
	send_key_hold_down(0x33, 2000);
	send_key_hold_down(0x34, 2000);
	send_key_hold_down(0x31, 1000);
	send_key_hold_down(VK_F1, 500);
	return 0;
}



int turn_and_run(const int time_in_millisec)
{
	HWND healfshwnd = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!healfshwnd)
	{
		healfshwnd = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	SetForegroundWindow(healfshwnd);
	//send A to change camera for X sec, then send W for Y sec
	send_key_hold_down(0x41, 500);
	send_key_hold_down(0x57, time_in_millisec);
	return 0;
}



void turn_run_and_jump(const int time_in_millisec)
{
	HWND healfshwnd = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!healfshwnd)
	{
		healfshwnd = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	SetForegroundWindow(healfshwnd);
	//send A to change camera for X sec, then send W for Y sec
	int time = time_in_millisec / 3;
	send_key_hold_down(0x41, 725);
	//for (int x = 0; x < 4; ++x)
	//{
	//	send_2_keys_and_hold_first(0x57, time, VK_SPACE);
	//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	//}
	//doing this in a for loop seems to break for some reason. Probably has to do with the compiler optimizations.
	send_2_keys_and_hold_first(0x57, time, VK_SPACE);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	send_2_keys_and_hold_first(0x57, time, VK_SPACE);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	send_2_keys_and_hold_first(0x57, time, VK_SPACE);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

}

//void send_command(std::vector<WORD> command)
//{
//	std::vector<WORD> position_command{ VK_RETURN, VK_OEM_2, 0x50 ,0x4F, 0x53 ,0x49 ,0x54, 0x49, 0x4F, 0x4E, VK_RETURN };
//
//	for (auto &key : position_command)
//	{
//		send_key_hold_down(key, 100);
//	}
//}


void correct_screen_position_for_monster_target(flyff::monster &mon_to_attack, char_position &pos, bool &sent_down, bool &sent_up, bool &sent_down_2, bool &sent_up_2)
{
	if ((mon_to_attack.y < pos.y - 5) && mon_to_attack.order < 10)
	{
		std::cout << "mon y is " << mon_to_attack.y << " player y is " << pos.y << " sending up arrow" << std::endl;
		send_key_hold_down(VK_UP, 200);
		sent_up = true;
		return;
	}
	if (mon_to_attack.y < pos.y - 5)
	{
		std::cout << "mon y is " << mon_to_attack.y << " player y is " << pos.y << " sending up arrow" << std::endl;
		send_key_hold_down(VK_UP, 150);
		sent_up_2 = true;
		return;
	}

	if ((mon_to_attack.y > pos.y + 5) && mon_to_attack.order < 10)
	{
		std::cout << "mon y is " << mon_to_attack.y << " player y is " << pos.y << " sending down arrow" << std::endl;
		send_key_hold_down(VK_DOWN, 200);
		sent_down = true;
		return;
	}
	if (mon_to_attack.y > pos.y + 5)
	{
		std::cout << "mon y is " << mon_to_attack.y << " player y is " << pos.y << " sending down arrow" << std::endl;
		send_key_hold_down(VK_DOWN, 150);
		sent_down_2 = true;
		return;
	}
}

void return_screen_position_for_monster_target(bool sent_down, bool sent_up, bool sent_down_2, bool sent_up_2)
{
	if (sent_up)
	{
		std::cout << "sent_up is true, sending down arrow" << std::endl;
		send_key_hold_down(VK_DOWN, 200);
		return;
	}
	if (sent_down)
	{
		std::cout << "sent_down is true, sending up" << std::endl;
		send_key_hold_down(VK_UP, 200);
		return;
	}
	if (sent_up_2)
	{
		std::cout << "sent_up_2 is true, sending down arrow" << std::endl;
		send_key_hold_down(VK_DOWN, 150);
		return;
	}
	if (sent_down_2)
	{
		std::cout << "sent_down_2 is true, sending up" << std::endl;
		send_key_hold_down(VK_UP, 150);
		return;
	}
}


bool check_for_valid_click(int y_click)
{
	//Checks if we are clicking the skill bar or the address bar on y axis
	//true is good
	RECT supportwindowrect_size;
	HWND supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!supportwindow_size)
	{
		supportwindow_size = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	GetWindowRect(supportwindow_size, &supportwindowrect_size);
	
	int address_bar_y = static_cast<int>(supportwindowrect_size.bottom * 0.1);
	if (y_click <= address_bar_y)
	{
		std::cout << "avoiding click of address bar" << std::endl;
			return false;
	}

	int skill_bar_y = static_cast<int>(supportwindowrect_size.bottom * 0.9);
	if (y_click >= skill_bar_y)
	{
		std::cout << "avoiding click of skill bar" << std::endl;
		return false;
	}

	return true;

}

void reset_screen_position()
{
	send_key_hold_down(VK_UP, 1000);
	send_key_hold_down(VK_DOWN, 400);
}
