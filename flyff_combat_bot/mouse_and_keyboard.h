// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#pragma once

#include <Windows.h>
//#include <iostream>
#include <vector>
//#include <string> 
#include <chrono>

#include "memory_reader.h"

using namespace std::chrono_literals;


void send_key_hold_down(char numpress, int time_in_millisec = 250);

int send_jump_and_run();

int send_jump();

int send_rightarrow_down();

int send_rightarrow_up();

int send_left_click(const int mousexloc, const int mouseyloc);

int send_left_click_delay(const int mousexloc, const int mouseyloc);

int move_mouse(const int mousexloc, const int mouseyloc);

int send_right_arrow();

int send_buffs();

int turn_and_run(const int time_in_millisec = 6000);

void send_command(std::vector<WORD> command);

void correct_screen_position_for_monster_target(flyff::monster &mon_to_attack, char_position &pos, bool &sent_down, bool &sent_up, bool &sent_down_2, bool &sent_up_2);

void return_screen_position_for_monster_target(bool sent_down, bool sent_up, bool sent_down_2, bool sent_up_2);

bool check_for_valid_click(int y_click);

void send_key_down(char key);

void send_key_up(char key);

void send_2_keys(char key1, int time_in_millisec, char key2);

void turn_run_and_jump(const int time_in_millisec);

void reset_screen_position();

void send_2_keys_and_hold_first(char key1, int time_in_millisec, char key2);