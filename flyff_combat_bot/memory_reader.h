// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#pragma once

#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>

#include "utils.h"
#include "game_window.h"
#include "trigger_time.h"
#include "log.h"
#include "memory.h"

struct flyff_process
{
	HANDLE pHandle = NULL;
	DWORD pid = 1;
	size_t bytesToRead = 0;
	size_t xbytesToRead = 0;
	size_t zbytesToRead = 0;
	size_t anglebytesToRead = 0;
	LPVOID queryFoundAddress = nullptr;
	LPVOID anglequeryFoundAddress = nullptr;
	LPVOID xqueryFoundAddress = nullptr;
	LPVOID zqueryFoundAddress = nullptr;
	LPVOID monster_id_query_found_address = nullptr;
	LPVOID monster_id_found_address = nullptr;
	size_t monster_id_bytes_to_read{};
	LPVOID player_target_pattern_address = nullptr;
	LPVOID player_target_address = nullptr;
	LPVOID player_id_address = nullptr;
	int player_id = 0;
	int monster_id = 0;
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
	float angle = 0.0;
};


struct char_position
{
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
	float angle = 0.0;
	float distance{ x + z };
};

namespace flyff
{
	class process
	{
		public:
			HANDLE pHandle = NULL;
			DWORD pid = 1;
			size_t bytesToRead{};
			size_t xbytesToRead{};
			size_t zbytesToRead{};
			size_t anglebytesToRead{};
			LPVOID queryFoundAddress = nullptr;
			LPVOID anglequeryFoundAddress = nullptr;
			LPVOID xqueryFoundAddress = nullptr;
			LPVOID zqueryFoundAddress = nullptr;
			LPVOID monster_id_query_found_address = nullptr;
			LPVOID monster_id_found_address = nullptr;
			size_t monster_id_bytes_to_read{};
			LPVOID player_target_pattern_address = nullptr;
			LPVOID player_target_address = nullptr;
			LPVOID player_id_address = nullptr;
			int player_id = 0;
			int monster_id = 0;
			float x = 0.0;
			float y = 0.0;
			float z = 0.0;
			float angle = 0.0;

		private:

	};

	class game
	{
		public:
			game(flyff::process proc);
		private:

	};

	class target_text
	{
	public:
	private:
	};

	class player
	{
		public:
			std::string name;
			int target = 0;
			LPVOID target_pattern_address = nullptr;
			LPVOID target_address = nullptr;
			//default player ID should be 1 in case there isn't a player we want to differentiate from 0
			int id = 1;
			bool combat = false;
			void setup_initial_target(const flyff::process &process);
			void setup_initial_pos(const flyff::process &process);
			void set_target(int target_id);
			void update_target();
			void set_position(flyff::window &win);
			void update_pos();
			void update_combat_status();
			void set_angle(float angle);
			void avoid_giant_on_map(flyff::window &win);
			void log_write(const std::string &msg);

			std::vector<LPVOID> find_all_text_targets(const flyff::process &process);

			LPVOID check_text_targets(const flyff::process &process, LPVOID add_to_check);

			bool check_if_text_target_is_bad_mon(std::vector<LPVOID> &all_text_targets);
			char_position pos;
			char_position target_pos;
			
			//player() = delete;

			//player(flyff::log &log_file) :log_file_(log_file) 
			//{
			//	std::cout << "player parameterized constructor invoked" << std::endl;
			//}

			void set_log_file(flyff::log *log_file);

			player() = default;

			player(const player &other) :
				name(other.name),
				target(other.target),
				target_pattern_address(other.target_pattern_address),
				target_address(other.target_address),
				id(other.id),
				combat(other.combat),
				pos(other.pos),
				target_pos(other.target_pos)
				//log_file_(other.log_file_)
			{
				std::cout << "player copy constructor invoked" << std::endl;

			}

			player &operator=(const player &other)
			{
				if (this != &other)
				{
					name = other.name;
					target = other.target;
					target_pattern_address = other.target_pattern_address;
					target_address = other.target_address;
					id = other.id;
					combat = other.combat;
					pos = other.pos;
					target_pos = other.target_pos;
					//log_file_ = other.log_file_;

					std::cout << "player copy assignment operator invoked" << std::endl;
				}
				return *this;
			}

			player(player &&other) noexcept :
				name(std::move(other.name)),
				target(std::move(other.target)),
				target_pattern_address(std::move(other.target_pattern_address)),
				target_address(std::move(other.target_address)),
				id(std::move(other.id)),
				combat(std::move(other.combat)),
				pos(std::move(other.pos)),
				target_pos(std::move(other.target_pos))
				//log_file_(other.log_file_)
			{
				//std::cout << "player move constructor invoked" << std::endl;
			}

			player &operator=(player &&other) noexcept
			{
				if (this != &other)
				{
					name = std::move(other.name);
					target = std::move(other.target);
					target_pattern_address = std::move(other.target_pattern_address);
					target_address = std::move(other.target_address);
					id = std::move(other.id);
					combat = std::move(other.combat);
					pos = std::move(other.pos);
					target_pos = std::move(other.target_pos);
					//log_file_ = other.log_file_;

					//std::cout << "player move assignment operator invoked" << std::endl;
				}
				return *this;
			}

			~player() = default;



		private:
			bool log_set_ = false;
			flyff::log *log_file_ = nullptr;
			std::vector<LPVOID> player_name_addresses_{};
			flyff::time set_position_time_;
			flyff::process player_target_proc;
			flyff::process pos_proc;
			std::array<unsigned char, 136> player_target_byte_pattern =
			{	//added 8 extra for the sliding window
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//7 (group 1)
				// in v3 8-15 are random
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F, //15 (group 2)
				//8-15 random
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //23 (group 3)
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F, //31 (group 4)
				//24-31 random
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //39 (group 5)
				// in v4 32-35 random
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //47 (group 6)
				//40-43 random
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //55 (group 7)
				//48-51 are the target
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //63 (group 8)
				//56-59 are also the target but we'll treat it as random, later we'll use it as active combat
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //71 (group 9)
				//64-67 are id of char you follow, we'll treat as random
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //79 (group 10)
				//72-75 random
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //87 (group 11)
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //95 (group 12)
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //103 (group 13)
				//group 13-15 are in v5
				//100 random
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //111 (group 14)
				//104 random seems to only show up in combat
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //119 (group 15)
				//112-113,116,118-119 random, seems to only show up in combat
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //127 (group 16)
				//added 8 extra for the sliding window
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //135 (group 17)
			};
	};

	class monster
	{
		public:
			LPVOID address = nullptr;
			LPVOID xaddress = nullptr;
			LPVOID zaddress = nullptr;
			float x = 0.0;
			float z = 0.0;
			float y = 0.0;
			float distance = x + z;
			int id = 0;
			int target = 0;
			float order = 255;
			void update_pos(const flyff::process &process);
			bool check_if_valid(const flyff::process &process);
			void update_target(const flyff::process &process);
			bool check_if_pet(const flyff::process &process);
			void set_log_file(flyff::log *log_file);
			void log_write(const std::string &msg);


			monster() = default;

			monster(const monster &other) :
				address(other.address),
				xaddress(other.xaddress),
				zaddress(other.zaddress),
				x(other.x),
				y(other.y),
				z(other.z),
				distance(other.distance),
				id(other.id),
				target(other.target),
				order(other.order)
			{
				std::cout << "monster copy constructor invoked" << std::endl;
			}

			monster &operator=(const monster &other)
			{
				if (this != &other)
				{
					address = other.address;
					xaddress = other.xaddress;
					zaddress = other.zaddress;
					x = other.x;
					y = other.y;
					z = other.z;
					distance = other.distance;
					id = other.id;
					target = other.target;
					order = other.order;

					std::cout << "monster copy assignment operator invoked" << std::endl;
				}
				return *this;
			}

			monster(monster &&other) noexcept :
				address(std::move(other.address)),
				xaddress(std::move(other.xaddress)),
				zaddress(std::move(other.zaddress)),
				x(std::move(other.x)),
				y(std::move(other.y)),
				z(std::move(other.z)),
				distance(std::move(other.distance)),
				id(std::move(other.id)),
				target(std::move(other.target)),
				order(std::move(other.order))
			{
				//std::cout << "monster move constructor invoked" << std::endl;
			}

			monster &operator=(monster &&other) noexcept
			{
				if (this != &other)
				{
					address = std::move(other.address);
					xaddress = std::move(other.xaddress);
					zaddress = std::move(other.zaddress);
					x = std::move(other.x);
					y = std::move(other.y);
					z = std::move(other.z);
					distance = std::move(other.distance);
					id = std::move(other.id);
					target = std::move(other.target);
					order = std::move(other.order);

					//std::cout << "monster move assignment operator invoked" << std::endl;
				}
				return *this;
			}

			~monster() = default;


		private:
			bool log_set_ = false;
			flyff::log *log_file_ = nullptr;
			bool valid_ = true;
			std::array<unsigned char, 56> monster_target_byte_pattern =
			{
				//around 1.5k after the monster ID is the monster's target
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F, //7
				//8-12 are random       
				//13-15 in v2                         
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //15
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F, //23
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F, //31
				//32-35 is the ID monster is following
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //39
				//40-43 are the target id that we ignore
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //47
				//48-51 are the target id that we match because it goes away after the mon dies
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //55
			};

	
			//v2
			std::array<unsigned char, 8> pet_byte_pattern =
			{
				0xC3,0xA0,0xFF,0xFF,0x3C,0x3C,0x3C,0xFF
			};
		};

		class target_string
		{
		public:

		private:
			std::array<unsigned char, 184> target_string_byte_pattern =
			{
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x61, 0x00, 0x00, 0x20, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
			};
		};
}





char_position get_char_position_from_string(std::string value);


HANDLE get_process_handle(const DWORD processID);


enum class direction
{
	north,
	south,
	east,
	west,
};


int set_player_angle(direction direction, flyff::process process);

void set_precise_player_angle(float angle,const flyff::process &process);

flyff::process find_flyff_string_in_pid(DWORD single_pid);

std::vector<DWORD> find_all_flyff_pids();

std::vector<flyff::monster> get_initial_monsters_on_field(const flyff::process &process, const std::array<unsigned char, 104> byte_pattern_to_match_monster_id);

void sort_and_filter_monsters_by_distance(const char_position &pos, std::vector<flyff::monster> &initial_monsters_on_field);


//this function should be used after initial monsters on field, whereas the delete version is for after selecting target
void remove_monster_from_field(int id, std::vector<flyff::monster> &initial_monsters_on_field);

//used to efficiently delete some monsters if they were set via a certain way
void delete_monster_from_field(std::vector<flyff::monster> &initial_monsters_on_field, const flyff::monster &mon_to_attack, bool mon_set);