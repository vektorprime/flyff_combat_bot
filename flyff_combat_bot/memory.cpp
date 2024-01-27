// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "memory.h"

namespace flyff
{
	memory::memory(LPVOID address, HANDLE proc_handle, size_t bytes_to_read) :
		bytes_to_read_(bytes_to_read),
		address_(address),
		proc_handle_(proc_handle)
	{
		buffer = VirtualAlloc(NULL, bytes_to_read_, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!buffer)
		{
			if (log_set_)
			{
				log_file_->write("ERROR: VirtualAlloc failed in memory::memory()");
			}
			std::cout << "ERROR: VirtualAlloc failed in memory::memory()" << std::endl;
			throw std::runtime_error("ERROR: VirtualAlloc failed in memory::memory()");
		}
	}

	void memory::read()
	{
		if (!ReadProcessMemory(proc_handle_, address_, buffer, bytes_to_read_, NULL))
		{
			if (log_set_)
			{
				log_file_->write("ERROR: Unable to read proc mem in memory::read()");
			}
			std::cout << "ERROR: Unable to read proc mem in memory::read()" << std::endl;
			throw std::runtime_error("ERROR: Unable to read proc mem in memory::read()");
		}
	}

	void memory::setup(LPVOID address, HANDLE proc_handle, size_t bytes_to_read)
	{
		bytes_to_read_ = bytes_to_read;
		address_ = address;
		proc_handle_ = proc_handle;
		buffer = VirtualAlloc(NULL, bytes_to_read_, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!buffer)
		{
			if (log_set_)
			{
				log_file_->write("ERROR: VirtualAlloc failed in memory::memory()");
			}
			std::cout << "ERROR: VirtualAlloc failed in memory::memory()" << std::endl;
			throw std::runtime_error("ERROR: VirtualAlloc failed in memory::memory()");
		}
	}


	void memory::set_log(flyff::log *log_file)
	{
		log_file_ = log_file;
		log_set_ = true;
	}

} //namespace flyff