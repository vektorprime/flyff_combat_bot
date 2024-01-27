// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#pragma once

#include <cstring>

namespace flyff
{
	namespace utils
	{
		void int_to_hex(int src_int, unsigned char *dst_char);

		void float_to_hex(float f1, unsigned char *f2);
	}

}
