#pragma once

#include <Windows.h>
#include <iostream>

#include "log.h"

namespace flyff
{

	class memory
	{
		public:

			LPVOID buffer = nullptr;

			memory() = default;

			memory(LPVOID address, HANDLE proc_handle, size_t bytes_to_read);
	

			memory(const memory &other) : 
				bytes_to_read_(other.bytes_to_read_),
				address_(other.address_),
				proc_handle_(other.proc_handle_),
				buffer(other.buffer)
			{

			}

			memory &operator=(const memory &other)
			{
				bytes_to_read_ = other.bytes_to_read_;
				address_ = other.address_;
				proc_handle_ = other.proc_handle_;
				buffer = other.buffer;

				return *this;
			}

			memory(memory &&other) noexcept :
				bytes_to_read_(other.bytes_to_read_),
				address_(other.address_),
				proc_handle_(other.proc_handle_),
				buffer(other.buffer)
			{

			}

			memory &operator=(memory &&other) noexcept
			{
				bytes_to_read_ = other.bytes_to_read_;
				address_ = other.address_;
				proc_handle_ = other.proc_handle_;
				buffer = other.buffer;

				return *this;
			}

			~memory()
			{
				//std::cout << "memory destructor invoked\n";
				VirtualFree(buffer, 0, MEM_RELEASE);
			}

			void read();

			void set_log(flyff::log* log_file);

			void setup(LPVOID address, HANDLE proc_handle, size_t bytes_to_read);

		private:
			flyff::log *log_file_ = nullptr;
			size_t bytes_to_read_ = 0;
			LPVOID address_ = nullptr;
			HANDLE proc_handle_ = nullptr;
			bool log_set_ = false;
	};
}