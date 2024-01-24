// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#pragma once

#include <vector>
#include <string> 
#include <fstream>

#include "log.h"


namespace flyff
{
	class config
	{
		public:

			config(const std::string &file, flyff::log &log_file);
			struct settings
			{
				std::string healer_name_value;
				std::string player_name_value;
				std::string enable_second_player_value;
				std::string debug_value;
				std::string mode_value;
				std::string x_click_offset_value;
				std::string y_click_offset_value;
				std::string restore_hp_value;
				std::string restore_mp_value;
				std::string restore_fp_value;
				std::string restore_hp_mp_fp_interval_value;
				std::string fight_self_buff_enabled_value;
				std::string fight_self_buff_skills_slot_value;
				std::string fight_self_buff_skills_intervals_value;
				std::string enable_attack_skill_value;
				std::string attack_skill_slot_value;

				std::string heal_type_value;
				std::string heal_enable_find_player_value;
				std::string heal_pause_between_interval_value;
				std::string heal_rebuff_interval_value;
				std::string heal_start_with_buffs_value;
				std::string heal_geb_rebuff_value;
				std::string heal_prev_rebuff_value;
				std::string heal_party_member_value;
				std::string heal_reactive_value;
				std::vector<int> fight_self_buff_skills_slot_value_all;
				std::vector<int> fight_self_buff_skills_intervals_value_all;
			};

			settings game_settings;

			

		private:
		
			std::ifstream get_config_file_handle_(const std::string &file);
			void remove_spaces_from_string_(std::string &fullstring);
			std::string get_config_value_(const std::string &config_string);
			std::string config_file_;
			std::ifstream config_file_handle_;
			std::vector<int> get_all_config_values_as_vect_(const std::string &config_string);
			flyff::log &log_file_;
			
	};
}