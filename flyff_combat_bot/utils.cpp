// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "utils.h"

namespace flyff
{
	namespace utils
	{
		void int_to_hex(int src_int, unsigned char *dst_char)
		{
			// Use memcpy to copy the bytes of of src_int to dst_char
			std::memcpy(dst_char, &src_int, sizeof(int));
		}
		void float_to_hex(float f1, unsigned char *f2)
		{

			// Use memcpy to copy the bytes of f1 into f2
			std::memcpy(f2, &f1, sizeof(float));
		}
	}

}


