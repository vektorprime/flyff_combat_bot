// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com



//used for image matching
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string> 
#include <fstream>
#include <thread>
#include <future>


//////used for OCR
//#include <leptonica/allheaders.h>
//#include <tesseract/baseapi.h>

//used for reading mem
#include <tlhelp32.h>
#include <iomanip>
#include <array>
#include <cmath>

#include <stdexcept>

//custom headers
#include "trigger_time.h"
#include "game_window.h"
#include "memory_reader.h"
#include "mouse_and_keyboard.h"
#include "image_matching.h"
#include "debug_image_matching.h"
#include "heal_and_buff.h"
#include "config_reader.h"
#include "ocr_matching.h"
#include "map_image_matching.h"
#include "log.h"


using namespace std::chrono_literals;

//new pattern as of 2/8
const std::array<unsigned char, 104> monster_on_map_byte_pattern_5 =
{
	//4-7 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //7
	0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01, //15
	//20-23 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //23
	//24-27 monster id
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //31
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //39
	//45-47 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //47
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //55
	//58-59, 61-63 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //63
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //71
	//77-79 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //79
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //87
	//93-95 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //95
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 //103
	//if matching this pattern it's 104 items
};



//flyff::player healer_player(log_file);
flyff::process flyff_proc{};




int main()
{
	flyff::log log_file("flyffcombat.log");
	//flyff::config config("flyffcombat.cfg", log_file);

	flyff::player player{};
	player.name = "OneShotMan";
	flyff::player healer_player{};

	player.set_log_file(&log_file);
	//healer_player.set_log_file(&log_file);

	std::vector<int> all_pet_id{};

	std::vector<flyff::monster> initial_monsters_on_field{};
	initial_monsters_on_field.reserve(65);



	std::vector<DWORD> all_pids = find_all_flyff_pids();
	for (int i = 0; i < all_pids.size(); i++)
	{

		flyff_proc = find_flyff_string_in_pid(all_pids[i]);
		std::cout << "checking if pid " << all_pids[i] << " is the flyff window\n";
		//if anything other than 1 is returned, we got the right PID
		if (flyff_proc.pid != 1)
		{
			break;
		}
	}

	
	player.setup_initial_pos(flyff_proc);
	player.setup_initial_target(flyff_proc);




	player.set_target(player.id);

	while (true)
	{
		initial_monsters_on_field = get_initial_monsters_on_field(flyff_proc, monster_on_map_byte_pattern_5);
		for (flyff::monster &mon : initial_monsters_on_field)
		{
			if (mon.check_if_valid(flyff_proc))
			{
				mon.update_pos(flyff_proc);
				mon.update_target(flyff_proc);

				std::cout << "Mon id " << mon.id << " address " << mon.address << " target " << mon.target << std::endl;

				player.set_target(mon.id);
				std::cin.ignore();
			}
			else
			{
				std::cout << "Mon ID " << mon.id << " is invalid, setting to 0\n";
				mon.id = 0;
			}
		}
	}


	


	return 0;
}