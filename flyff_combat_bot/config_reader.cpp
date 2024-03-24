// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "config_reader.h"

#include <iostream>
#include <fstream>
#include <string>


void flyff::config::remove_spaces_from_string_(std::string &fullstring)
{
	while (fullstring.std::string::find(' ') != std::string::npos)
	{
		std::size_t pos = fullstring.std::string::find(' ');
		fullstring.std::string::erase(pos, 1);
	}
}



std::string flyff::config::get_config_value_(const std::string &config_string)
{
	config_file_handle_.seekg(0, std::ios::beg);
	std::string value;
	for (std::string line; std::getline(config_file_handle_, line);)
	{
		std::string_view line_view = line;
		//find the string line, must make sure it's not npos or else it won't match right
		//some lines may contain spaces and we don't want to error on those
		if (line_view.find(config_string) != std::string::npos)
		{
			//find the = in the string
			std::size_t pos = line_view.find('=');
			//split the string after the =, creating a value
			pos++;
			//value = line.std::string::substr(pos);
			value = std::string(line_view.substr(pos));
			//check for spaces, and remove if found
			remove_spaces_from_string_(value);
			break;
		}
	}
	if (value.empty())
	{
		log_file_.write("ERROR: empty config value for " + config_string);
	}
	return value;
}

std::ifstream flyff::config::get_config_file_handle_(const std::string &file)
{
	std::ifstream open_file(file);
	if (!open_file.is_open())
	{
		log_file_.write("ERROR: unable to open config file " + file);
		std::cerr << "Failed to open the file.\n";
	}
	return open_file;
}


std::vector<int> flyff::config::get_all_config_values_as_vect_(const std::string &config_string)
{
	int config_string_length = config_string.length();
	std::vector<int> vect{};
	std::vector<int> vect_pos{};


	//get all of the comma locations
	for (int i = 0; i < config_string.size(); i++)
	{
		//char compare = ',';
		if (config_string[i] == ',')
		{
			//char slot = config_string[i];
			vect_pos.push_back(i);
			//std::cout << "the comma positon is " << i << std::endl;
		}
	}

	//first skill slot
	 //assume the first skill slot is before the comma, the last is after the comma
	/*int vect_first_comma = config_string.std::string::find_first_of(',');
	std::string vect_first_value = config_string.std::string::substr(0, vect_first_comma);*/
	std::string vect_first_value = config_string.std::string::substr(0, config_string.std::string::find_first_of(','));
	//convert string to int and add to int array
	vect.push_back(std::stoi(vect_first_value));

	//everything else in between
	//take 1 less than so we only get the in between values
	for (int i = 0; i < vect_pos.size() - 1; ++i)
	{
		//int i1 = ;
		////std::cout << "c1 is " << i1 << std::endl;
		//int i2 = vect_pos[i + 1];
		//std::cout << "c2 is " << i2 << std::endl;
		std::string s = config_string.std::string::substr((vect_pos[i]) + 1, ((vect_pos[i + 1]) - (vect_pos[i])) - 1);
		// std::cout << "s is " << s << std::endl;
		 //convert the skill to a int and add it to the vector
		vect.push_back(std::stoi(s));
	}

	//last skill slot
	int vect_last_comma = config_string.std::string::find_last_of(',');
	// add one so we don't retreive the comma, only the string
	++vect_last_comma;
	std::string vect_last_value = config_string.std::string::substr(vect_last_comma, config_string_length);
	//convert string to int and add to int array
	vect.push_back(stoi(vect_last_value));


	return vect;
}

flyff::config::config(const std::string &file, flyff::log &log_file) : config_file_{ file }, log_file_(log_file)
{
	config_file_handle_ = get_config_file_handle_(config_file_);

	game_settings.enable_second_player_value = get_config_value_("enable_second_player");
	game_settings.healer_name_value = get_config_value_("healer_name");
	game_settings.player_name_value = get_config_value_("player_name");
	game_settings.debug_value = get_config_value_("debug");
	game_settings.mode_value = get_config_value_("mode");
	game_settings.x_click_offset_value = get_config_value_("x_click_offset");
	game_settings.y_click_offset_value = get_config_value_("y_click_offset");
	game_settings.restore_hp_value = get_config_value_("restore_hp_bar");
	game_settings.restore_mp_value = get_config_value_("restore_mp_bar");
	game_settings.restore_fp_value = get_config_value_("restore_fp_bar");
	game_settings.restore_hp_mp_fp_interval_value = get_config_value_("restore_hp_mp_fp_interval");
	game_settings.fight_self_buff_enabled_value = get_config_value_("fight_self_buff_enabled");
	game_settings.fight_self_buff_skills_slot_value = get_config_value_("fight_self_buff_skills_slot");

	game_settings.fight_self_buff_skills_intervals_value = get_config_value_("fight_self_buff_skills_intervals");
	game_settings.enable_attack_skill_value = get_config_value_("enable_attack_skill");
	game_settings.attack_skill_slot_value = get_config_value_("attack_skill_slot");

	game_settings.heal_enable_find_player_value = get_config_value_("heal_enable_find_player");
	game_settings.heal_type_value = get_config_value_("heal_type");
	game_settings.heal_pause_between_interval_value = get_config_value_("heal_pause_between_interval");
	game_settings.heal_rebuff_interval_value = get_config_value_("heal_rebuff_interval");
	game_settings.heal_start_with_buffs_value = get_config_value_("heal_start_with_buffs");
	game_settings.heal_geb_rebuff_value = get_config_value_("heal_geb_rebuff");
	game_settings.heal_prev_rebuff_value = get_config_value_("heal_prev_rebuff");
	game_settings.heal_party_member_value = get_config_value_("heal_party_member");
	game_settings.heal_reactive_value = get_config_value_("heal_reactive");
	game_settings.fight_self_buff_skills_slot_value_all = get_all_config_values_as_vect_(game_settings.fight_self_buff_skills_slot_value);
	game_settings.fight_self_buff_skills_intervals_value_all = get_all_config_values_as_vect_(game_settings.fight_self_buff_skills_intervals_value);
	//game_settings.fight_self_buff_quantity_value = '0' + game_settings.fight_self_buff_skills_slot_value_all.size();
	config_file_handle_.close();

}