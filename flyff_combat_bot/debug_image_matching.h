// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#pragma once

#include "game_window.h"

cv::Mat debug_map(flyff::window &win, const int xoffset = 0, const int yoffset = 0);

cv::Mat debug_window(flyff::window &win, const double xoffset = 0, const double yoffset = 0, int windowwidth = 150, int windowheight = 150);

cv::Mat debug_hp_mp_fp(flyff::window &win, const double x = 0, const double y = 0.085, const double width = 0.23, const double height = 0.1);

cv::Mat debug_alert_text(flyff::window &win, const double x = 0.3, const double y = 0.5, const double width = 0.3, const double height = 0.3);

cv::Mat debug_combat(flyff::window &win, const double x = 0.8, const double y = 0.25, const double width = 0.2, const double height = 0.1);
