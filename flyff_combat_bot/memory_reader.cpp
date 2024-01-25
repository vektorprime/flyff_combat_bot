// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "memory_reader.h"

#include "mouse_and_keyboard.h"
#include "map_image_matching.h"

#include <chrono>
#include <array>
#include <tlhelp32.h>
#include <cstddef>

#include <algorithm>
#include <cmath>




HANDLE get_process_handle(const DWORD processID)
{
	HANDLE processHandle{};

	if (processHandle == NULL)
	{
		//get the proc
		processHandle = OpenProcess(PROCESS_ALL_ACCESS, true, processID);
		if (!processHandle)
		{
			throw std::invalid_argument("Unable to get process handle");

		}
		std::cout << "Got the process Handle " << processHandle << std::endl;
		return processHandle;
	}
}

void allocate_memory_buffer(size_t bytesToRead, LPVOID &queryBaseAddress, LPVOID &bufferRead)
{
	bufferRead = VirtualAlloc(NULL, bytesToRead, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!bufferRead)
	{
		// No need to run VirtualFree because the alloc failed
		// Increment the base so we can keep trying
		queryBaseAddress = (LPVOID)((uintptr_t)queryBaseAddress + (uint64_t)bytesToRead);
	}
}



//set char angle via memory edit of chrome
int set_player_angle(direction direction, flyff::process process)
{

	std::array <unsigned char, 4> float_to_set{};

	if (direction == direction::south)
	{
		std::cout << "setting angle to 0 (south)" << std::endl;
		float_to_set = { 0x00, 0x00, 0x00, 0x00 };
	}
	if (direction == direction::north)
	{
		std::cout << "setting angle to 180 (north)" << std::endl;
		float_to_set = { 0x00, 0x00, 0x34, 0x43 };
	}
	if (direction == direction::west)
	{
		std::cout << "setting angle to 270 (west)" << std::endl;
		float_to_set = { 0x00, 0x00, 0x87, 0x43 };
	}
	if (direction == direction::east)
	{
		std::cout << "setting angle to 90 (east)" << std::endl;
		float_to_set = { 0x00, 0x00, 0xB4, 0x42 };
	}
	if (!WriteProcessMemory(process.pHandle, process.anglequeryFoundAddress, &float_to_set, sizeof(float), NULL))
	{
		std::cout << "Unable to write process mem" << std::endl;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return 0;
}



void set_precise_player_angle(float angle, const flyff::process &process)
{
	unsigned char f2[4];
	flyff::utils::float_to_hex(angle, f2);

	if (!WriteProcessMemory(process.pHandle, process.anglequeryFoundAddress, &f2, sizeof(float), NULL))
	{
		std::cout << "Unable to write process mem" << std::endl;
	}
}

void flyff::player::set_angle(float angle)
{
	unsigned char f2[4]; 
	flyff::utils::float_to_hex(angle, f2);

	if (!WriteProcessMemory(pos_proc.pHandle, pos_proc.anglequeryFoundAddress, &f2, sizeof(float), NULL))
	{
		std::cout << "Unable to write process mem in set_angle" << std::endl;
	}
}


flyff::process find_flyff_string_in_pid(DWORD single_pid)
{
	flyff::process item;
	DWORD processID = single_pid;
	LPVOID queryBaseAddress = nullptr;
	size_t bytesToRead = 0;
	LPVOID bufferRead = nullptr;

	
	
	item.pHandle = get_process_handle(processID);
	//processHandle = get_process_handle(processID);

	//avoid creating these inside of the while loop
	//This is the best string to search for because char status is usually up
	std::array<unsigned char, 7> flyff_string = { ' ','-' ,' ','L','v',':',' ' };
	std::array<unsigned char, 7> possible_match{};

	while (true)
	{
		MEMORY_BASIC_INFORMATION queryBuffer;

		if (!VirtualQueryEx(item.pHandle, queryBaseAddress, &queryBuffer, sizeof(queryBuffer)))
		{
			//std::cout << "VirtualQueryEx failed returning blank item" << std::endl;
			{
				return item;
			}
		}

		if (queryBaseAddress == NULL)
		{
			queryBaseAddress = queryBuffer.BaseAddress;
		}

		bytesToRead = queryBuffer.RegionSize;

		/*	we need to keep trying here because we aren't sure where we are in memory, eventually we'll find the area
		The region we find the data in is always the number below. If we don't specifically match the condition
		we end up allocating and freeing a lot of memory regions which introduces 2-3 sec of delay*/
		//268MB is the exact size of memory that the data we want uses
		if (bytesToRead == 268'435'456)
		{
			bufferRead = VirtualAlloc(NULL, bytesToRead, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (!bufferRead)
			{
				// No need to run VirtualFree because the alloc failed
				// Increment the base so we can keep trying
				queryBaseAddress = (LPVOID)((uintptr_t)queryBaseAddress + bytesToRead);
				continue;
			}

			//std::cout << "the Region size is " << queryBuffer.RegionSize << std::endl;


			if (!ReadProcessMemory(item.pHandle, queryBaseAddress, bufferRead, bytesToRead, NULL))
			{
				// Free the allocated memory
				VirtualFree(bufferRead, 0, MEM_RELEASE);
				// Increment the base
				queryBaseAddress = (LPVOID)((uintptr_t)queryBaseAddress + bytesToRead);
				continue;
			}
		}
		else
		{
			queryBaseAddress = (LPVOID)((uintptr_t)queryBaseAddress + bytesToRead);
			continue;
		}



		//go through each byte in the buffer bufferRead by counting bytesToRead
		for (uint64_t byte = 0; byte >= 0 && byte < bytesToRead; ++byte)
		{

			// if we find the O char get the rest of the chars and compare
			if (static_cast<unsigned char *>(bufferRead)[byte] == flyff_string[0])
			{
				if (byte + 7 < bytesToRead)
				{
					//std::wcout << "Found O, checking the rest" << std::endl;
					possible_match[0] = static_cast<unsigned char *>(bufferRead)[byte];
					possible_match[1] = static_cast<unsigned char *>(bufferRead)[(byte + 1)];
					possible_match[2] = static_cast<unsigned char *>(bufferRead)[(byte + 2)];
					possible_match[3] = static_cast<unsigned char *>(bufferRead)[(byte + 3)];
					possible_match[4] = static_cast<unsigned char *>(bufferRead)[(byte + 4)];
					possible_match[5] = static_cast<unsigned char *>(bufferRead)[(byte + 5)];
					possible_match[6] = static_cast<unsigned char *>(bufferRead)[(byte + 6)];
				}

				if (flyff_string == possible_match)
				{
					std::cout << "Found the string '- Lv:' at address " << queryBaseAddress << " and the offset is " << byte << " bytes" << std::endl;
					std::cout << "Final address is " << (LPVOID)((uintptr_t)queryBaseAddress + byte) << std::endl;
					byte = bytesToRead;
					item.pid = single_pid;
					item.bytesToRead = bytesToRead;
					item.queryFoundAddress = queryBaseAddress;
					std::cout << "The region size where we found the string is " << bytesToRead << std::endl;
					VirtualFree(bufferRead, 0, MEM_RELEASE);
					return item;
				}
			}
		}

		// std::cout << "finished in region " << bytesToRead << " moving on to next region \n" << std::endl;

	  // Free the allocated memory
		VirtualFree(bufferRead, 0, MEM_RELEASE);
		// Increment the base
		queryBaseAddress = (LPVOID)((uintptr_t)queryBaseAddress + bytesToRead);
	}

}


std::vector<DWORD> find_all_flyff_pids()
{
	std::vector<DWORD> all_pids{};
	DWORD parentProcessId = 0;

	HWND flyff_base_handle = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
	if (!flyff_base_handle)
	{
		flyff_base_handle = FindWindowW(NULL, L"Flyff Universe - Chromium");
	}
	GetWindowThreadProcessId(flyff_base_handle, &parentProcessId);


	// Create a snapshot of all running processes
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Failed to create a process snapshot." << std::endl;
	}

	// Initialize the process entry structure
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	// Iterate through all processes in the snapshot
	if (Process32First(hSnapshot, &processEntry))
	{
		do
		{
			if (processEntry.th32ParentProcessID == parentProcessId)
			{
				std::cout << "Child Process ID: " << processEntry.th32ProcessID << std::endl;
				all_pids.push_back(processEntry.th32ProcessID);

			}
		} while (Process32Next(hSnapshot, &processEntry));
	}
	else
	{
		std::cerr << "Failed to enumerate processes." << std::endl;
	}

	// Close the process snapshot handle
	CloseHandle(hSnapshot);

	return all_pids;
}


//////


void flyff::player::setup_initial_pos(const flyff::process &process)
{
	pos_proc.pid = process.pid;
	pos_proc.queryFoundAddress = process.queryFoundAddress;
	LPVOID queryBaseAddress = process.queryFoundAddress;
	size_t queryFoundRegionSize = process.bytesToRead;
	size_t bytesToRead = process.bytesToRead;
	//LPVOID bufferRead = nullptr;
	bool angle_found = false;
	bool x_pos_found = false;
	uint64_t x_pos_byte_loc = 0;
	uint64_t z_pos_byte_loc = 0;
	bool z_pos_found = false;
	std::array<unsigned char, 4> x_bytes{};
	std::array<unsigned char, 4> z_bytes{};
	std::array<unsigned char, 4> angle_bytes{};
	uint64_t outside_byte = 0;
	bool continue_early = false;
	//pos_proc.pHandle = process.pHandle;
	pos_proc.pHandle = get_process_handle(process.pid);

	std::cout << "Starting initial pos finder" << std::endl;
	if (log_file_->log_level_is_verbose())
	{
		log_write("Starting initial pos finder");
	}
	flyff::memory mem(process.queryFoundAddress, pos_proc.pHandle, bytesToRead);
	mem.read();

	while (!angle_found)
	{
		//angle finder

		//std::cout << "Read the process memory and moved it to the buffer" << std::endl;

		//std::cout << "checking for value in the buffer" << std::endl;
		//54 byte array for match, z coord is 4 bytes starting at 4 bytes into the array below, I marked it 0x00 because I won't compare those values
		std::array<unsigned char, 72> bytes_to_match_angle =
		{
			//first 4 bytes are the angle
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F,
			0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x00,
			0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00
		};

		uint64_t bufferSize = bytesToRead;
		//go through each byte in the buffer bufferRead by counting bytesToRead
		//72 bytes is how big the match angle pattern is
		std::array<unsigned char, 72 > check_bytes_after_angle{};
		//I start at 15 mil bytes because I never see the data before this value
		//I check every 4 bytes to speed it up further
		for (uint64_t byte = 2'000'000; byte < bufferSize; byte++)
		{

			//Get the first 72 bytes and compare it to our pattern
			for (uint64_t possibe_byte = 0; possibe_byte < 72; possibe_byte++)
			{
				// std::cout << "assigning b position of check_bytes_after_angle at " << b << std::endl;
				if (byte + possibe_byte >= 0 && byte + possibe_byte < bufferSize)
				{
					//std::cout << "adding bytes in check_bytes_after_angle " << possibe_byte << std::endl;
					check_bytes_after_angle[possibe_byte] = static_cast<unsigned char *>(mem.buffer)[byte + possibe_byte];
					//these are unique bytes compared to just matching 0x00, which will always match.
					//if bytes 10 and 11 don't match, move on
					if (possibe_byte == 11)
					{
						if (check_bytes_after_angle[10] != bytes_to_match_angle[10] &&
							check_bytes_after_angle[11] != bytes_to_match_angle[11])
						{
							continue_early = true;
							break;
						}
					}



				}
				else
				{
					log_write("ERROR: Unable to assign check_bytes_after_angle in initial check");
					//the initial function should continue instead of freeing virtual aloc
					std::cout << "ERROR: Unable to assign check_bytes_after_angle in initial check" << std::endl;
					throw std::runtime_error("ERROR: Unable to assign check_bytes_after_angle in initial check");
					//continue;
				}
			}
			if (continue_early)
			{
				continue_early = false;
				continue;
			}
			//these are the mask values to replace on the possible match, these values don't matter, but they're random
			//angle here
			check_bytes_after_angle[0] = 0x00;
			check_bytes_after_angle[1] = 0x00;
			check_bytes_after_angle[2] = 0x00;
			check_bytes_after_angle[3] = 0x00;

			check_bytes_after_angle[20] = 0x00;
			check_bytes_after_angle[21] = 0x00;
			check_bytes_after_angle[22] = 0x00;
			check_bytes_after_angle[23] = 0x00;

			check_bytes_after_angle[24] = 0x00;
			check_bytes_after_angle[25] = 0x00;
			check_bytes_after_angle[26] = 0x00;
			check_bytes_after_angle[27] = 0x00;

			check_bytes_after_angle[32] = 0x00;
			check_bytes_after_angle[33] = 0x00;
			check_bytes_after_angle[34] = 0x00;
			check_bytes_after_angle[35] = 0x00;

			check_bytes_after_angle[49] = 0x00;
			check_bytes_after_angle[50] = 0x00;
			check_bytes_after_angle[51] = 0x00;
			check_bytes_after_angle[52] = 0x00;
			check_bytes_after_angle[53] = 0x00;
			check_bytes_after_angle[54] = 0x00;
			check_bytes_after_angle[55] = 0x00;

			check_bytes_after_angle[66] = 0x00;
			check_bytes_after_angle[67] = 0x00;

			check_bytes_after_angle[68] = 0x00;
			check_bytes_after_angle[69] = 0x00;
			check_bytes_after_angle[70] = 0x00;
			check_bytes_after_angle[71] = 0x00;

			if (check_bytes_after_angle == bytes_to_match_angle)
			{
				x_pos_byte_loc = byte;
				z_pos_byte_loc = byte;
				//the float is after the 45 00 00 00 00.
				angle_bytes[0] = static_cast<unsigned char *>(mem.buffer)[byte + 0];
				angle_bytes[1] = static_cast<unsigned char *>(mem.buffer)[byte + 1];
				angle_bytes[2] = static_cast<unsigned char *>(mem.buffer)[byte + 2];
				angle_bytes[3] = static_cast<unsigned char *>(mem.buffer)[byte + 3];
				memcpy(&target_pos.angle, &angle_bytes, sizeof(float));
				std::cout << "angle_val is " << target_pos.angle << std::endl;
				angle_found = true;
				pos_proc.anglequeryFoundAddress = (LPVOID)((uintptr_t)queryBaseAddress + byte);
				pos_proc.anglebytesToRead = byte;
				pos_proc.bytesToRead = bytesToRead;
				std::cout << "Found the angle_val at address " << queryBaseAddress << " and the offset is " << byte << " bytes" << std::endl;
				pos_proc.queryFoundAddress = (LPVOID)((uintptr_t)queryBaseAddress + byte);
				std::cout << "The address is " << pos_proc.queryFoundAddress << std::endl;
			}

			if (angle_found)
			{
				break;
			}

			outside_byte = byte;
			//std::cout << "compared check_bytes_after and bytes_to_match_angle" << std::endl;
		}
		queryBaseAddress = (LPVOID)((uintptr_t)queryBaseAddress + (uint64_t)outside_byte);
		//std::cout << " the current address is  " << queryBaseAddress << std::endl;
	}

	//starting x pos finder
	//std::cout << "starting x pos finder" << std::endl;
	while (!x_pos_found)
	{
		x_bytes[0] = static_cast<unsigned char *>(mem.buffer)[x_pos_byte_loc - 16];
		x_bytes[1] = static_cast<unsigned char *>(mem.buffer)[x_pos_byte_loc - 15];
		x_bytes[2] = static_cast<unsigned char *>(mem.buffer)[x_pos_byte_loc - 14];
		x_bytes[3] = static_cast<unsigned char *>(mem.buffer)[x_pos_byte_loc - 13];
		memcpy(&target_pos.x, &x_bytes, sizeof(float));
		if (target_pos.x > 0 && target_pos.x < 25000)
		{
			pos_proc.xqueryFoundAddress = (LPVOID)((uintptr_t)pos_proc.anglequeryFoundAddress - (uint64_t)16);
			pos_proc.xbytesToRead = bytesToRead;
			x_pos_found = true;

		}
	}

	//starting x pos finder
	//std::cout << "starting x pos finder" << std::endl;
	std::array<unsigned char, 4> y_bytes{};
	y_bytes[0] = static_cast<unsigned char *>(mem.buffer)[x_pos_byte_loc - 12];
	y_bytes[1] = static_cast<unsigned char *>(mem.buffer)[x_pos_byte_loc - 11];
	y_bytes[2] = static_cast<unsigned char *>(mem.buffer)[x_pos_byte_loc - 10];
	y_bytes[3] = static_cast<unsigned char *>(mem.buffer)[x_pos_byte_loc - 9];
	memcpy(&target_pos.y, &y_bytes, sizeof(float));

	//starting z pos finder
	//std::cout << "starting Z pos finder" << std::endl;
	while (!z_pos_found)
	{
		z_bytes[0] = static_cast<unsigned char *>(mem.buffer)[z_pos_byte_loc - 8];
		z_bytes[1] = static_cast<unsigned char *>(mem.buffer)[z_pos_byte_loc - 7];
		z_bytes[2] = static_cast<unsigned char *>(mem.buffer)[z_pos_byte_loc - 6];
		z_bytes[3] = static_cast<unsigned char *>(mem.buffer)[z_pos_byte_loc - 5];
		memcpy(&target_pos.z, &z_bytes, sizeof(float));
		if (target_pos.z > 0 && target_pos.z < 25000)
		{
			z_pos_found = true;
		}
	}
	std::cout << "Player POS: X " << target_pos.x << " Y " << target_pos.y << " Z " << target_pos.z << " angle " << target_pos.angle << std::endl;
}



void flyff::player::update_pos()
{

	std::array<unsigned char, 4> x_bytes{};
	std::array<unsigned char, 4> z_bytes{};
	std::array<unsigned char, 4> angle_bytes{};


	HANDLE processHandle = nullptr;

	processHandle = pos_proc.pHandle;
	//angle finder
	if (log_file_->log_level_is_verbose())
	{
		log_write("Starting precise pos finder");
	}
	std::cout << "Starting precise pos finder" << std::endl;

	//already have a process handle, no need to get another

	flyff::memory mem(pos_proc.xqueryFoundAddress, pos_proc.pHandle, 49);
	mem.read();

	//only allocate enough to read what we want. X and Z are before the angle location. 

	x_bytes[0] = static_cast<unsigned char *>(mem.buffer)[0];
	x_bytes[1] = static_cast<unsigned char *>(mem.buffer)[1];
	x_bytes[2] = static_cast<unsigned char *>(mem.buffer)[2];
	x_bytes[3] = static_cast<unsigned char *>(mem.buffer)[3];
	memcpy(&pos.x, &x_bytes, sizeof(float));

	std::array<unsigned char, 4> y_bytes{};
	y_bytes[0] = static_cast<unsigned char *>(mem.buffer)[4];
	y_bytes[1] = static_cast<unsigned char *>(mem.buffer)[5];
	y_bytes[2] = static_cast<unsigned char *>(mem.buffer)[6];
	y_bytes[3] = static_cast<unsigned char *>(mem.buffer)[7];
	memcpy(&pos.y, &y_bytes, sizeof(float));

	z_bytes[0] = static_cast<unsigned char *>(mem.buffer)[8];
	z_bytes[1] = static_cast<unsigned char *>(mem.buffer)[9];
	z_bytes[2] = static_cast<unsigned char *>(mem.buffer)[10];
	z_bytes[3] = static_cast<unsigned char *>(mem.buffer)[11];
	memcpy(&pos.z, &z_bytes, sizeof(float));
	if (pos.z > 0 && pos.z < 25000)
	{
		pos.distance = pos.x + pos.z;
	}

	angle_bytes[0] = static_cast<unsigned char *>(mem.buffer)[16];
	angle_bytes[1] = static_cast<unsigned char *>(mem.buffer)[17];
	angle_bytes[2] = static_cast<unsigned char *>(mem.buffer)[18];
	angle_bytes[3] = static_cast<unsigned char *>(mem.buffer)[19];
	memcpy(&pos.angle, &angle_bytes, sizeof(float));
	std::cout << "Player POS: X " << pos.x << " Y " << pos.y << " Z " << pos.z << " angle " << pos.angle << std::endl;
	//if we reach the end that means we found the angle, x, and z. We should free resources and return the complete object.
	if (pos.x == 0 || pos.y == 0 | pos.z == 0 || pos.angle == 0)
	{
		log_write("Starting precise pos finder");
		throw std::runtime_error("ERROR: player pos is 0, exiting");

	}
}



void flyff::player::set_position(flyff::window &win)
{
	set_position_time_.set_interval(30);
	set_position_time_.set_start_time();
	//char_position precise_process = get_precise_player_pos(process);
	update_pos();
	float current_x = pos.x;
	float current_z = pos.z;
	float target_x = target_pos.x;
	float target_z = target_pos.z;
	std::cout << "Current X is " << current_x << std::endl;
	std::cout << "Current Z is " << current_z << std::endl;
	std::cout << "Target X is " << target_x << std::endl;
	std::cout << "Target Z is " << target_z << std::endl;

	if (target_x == 0 || target_z == 0 || current_x == 0 || current_z == 0)
	{
		return;
	}

	//used to break out of the while if we are stuck or dead
	uint64_t pos_not_moved_count = 0;
	float last_current_x = 0.0;
	float last_current_z = 0.0;

	bool x_checked = false;
	bool z_checked = false;

	while (current_x > target_x)
	{
		if (set_position_time_.check_if_time_elapsed())
		{
			std::cout << "setting position for too long, exiting set set_position" << std::endl;
			return;
		}
		std::cout << "Current X is greater than target X" << std::endl;
		float x_diff = target_x - current_x;
		float z_diff = target_z - current_z;
		float target_angle = atan2(z_diff, x_diff) * 180 / 3.14159265f;
		std::cout << "target_angle is " << target_angle << std::endl;
		target_angle += 90;
		std::cout << "adjusted target_angle is " << target_angle << std::endl;
		//set_precise_player_angle(target_angle, pos_proc);
		set_angle(target_angle);
		//send W and space to jump and pass obstacles
		send_jump_and_run();
		avoid_giant_on_map(win);
		std::this_thread::sleep_for(std::chrono::milliseconds(500ms));
		update_pos();
		current_x = pos.x;
		current_z = pos.z;
		std::cout << "Current X is " << current_x << std::endl;
		std::cout << "Target X is " << target_x << std::endl;
		//if the pos hasn't changed after 20 checks, break out of while loop
		if (current_x == last_current_x && current_z == last_current_z)
		{
			pos_not_moved_count++;
			if (pos_not_moved_count == 20)
			{
				break;
			}
		}
		last_current_x = current_x;
		last_current_z = current_z;

		//adding this so that if we go over we don't get stuck in a loop
		if (current_x < target_x)
		{
			x_checked = true;
			break;
		}
	}

	while (current_z > target_z)
	{
		if (set_position_time_.check_if_time_elapsed())
		{
			std::cout << "setting position for too long, exiting set set_position" << std::endl;
			return;
		}
		std::cout << "Current Z is greater than target Z" << std::endl;
		float x_diff = target_x - current_x;
		float z_diff = target_z - current_z;
		float target_angle = atan2(z_diff, x_diff) * 180 / 3.14159265f;
		std::cout << "target_angle is " << target_angle << std::endl;
		target_angle += 90;
		std::cout << "adjusted target_angle is " << target_angle << std::endl;
		//set_precise_player_angle(target_angle, pos_proc);
		set_angle(target_angle);
		send_jump_and_run();
		avoid_giant_on_map(win);
		std::this_thread::sleep_for(std::chrono::milliseconds(500ms));
		update_pos();
		current_x = pos.x;
		current_z = pos.z;
		std::cout << "Current X is " << current_x << std::endl;
		std::cout << "Current Z is " << current_z << std::endl;
		std::cout << "Target Z is " << target_z << std::endl;
		//if the pos hasn't changed after 20 checks, break out of while loop
		if (current_x == last_current_x && current_z == last_current_z)
		{
			pos_not_moved_count++;
			if (pos_not_moved_count == 20)
			{
				break;
			}
		}
		last_current_x = current_x;
		last_current_z = current_z;

		//adding this so that if we go over we don't get stuck in a loop
		if (current_z < target_z)
		{
			z_checked = true;
			break;
		}
	}


	while (current_x < target_x && x_checked == false)
	{
		if (set_position_time_.check_if_time_elapsed())
		{
			std::cout << "setting position for too long, exiting set set_position" << std::endl;
			return;
		}
		std::cout << "Current X is less than target X" << std::endl;
		float x_diff = target_x - current_x;
		float z_diff = target_z - current_z;
		float target_angle = atan2(z_diff, x_diff) * 180 / 3.14159265f;
		std::cout << "target_angle is " << target_angle << std::endl;
		target_angle += 90;
		std::cout << "adjusted target_angle is " << target_angle << std::endl;
		//set_precise_player_angle(target_angle, pos_proc);
		set_angle(target_angle);
		send_jump_and_run();
		avoid_giant_on_map(win);
		//pause so the movement can finish before we get the cords
		std::this_thread::sleep_for(std::chrono::milliseconds(500ms));
		update_pos();
		current_x = pos.x;
		current_z = pos.z;
		std::cout << "Current X is " << current_x << std::endl;
		std::cout << "Target X is " << target_x << std::endl;
		//if the pos hasn't changed after 20 checks, break out of while loop
		if (current_x == last_current_x && current_z == last_current_z)
		{
			pos_not_moved_count++;
			if (pos_not_moved_count == 20)
			{
				break;
			}
		}
		last_current_x = current_x;
		last_current_z = current_z;
		//adding this so that if we go over we don't get stuck in a loop
		if (current_x > target_x)
		{
			break;
		}
	}


	while (current_z < target_z && z_checked == false)
	{
		if (set_position_time_.check_if_time_elapsed())
		{
			std::cout << "setting position for too long, exiting set set_position" << std::endl;
			return;
		}
		std::cout << "Current Z is less than target Z" << std::endl;
		float x_diff = target_x - current_x;
		float z_diff = target_z - current_z;
		float target_angle = atan2(z_diff, x_diff) * 180 / 3.14159265f;
		std::cout << "target_angle is " << target_angle << std::endl;
		target_angle += 90;
		std::cout << "adjusted target_angle is " << target_angle << std::endl;
		//set_precise_player_angle(target_angle, pos_proc);
		set_angle(target_angle);
		send_jump_and_run();
		avoid_giant_on_map(win);
		//pause so the movement can finish before we get the cords
		std::this_thread::sleep_for(std::chrono::milliseconds(500ms));
		update_pos();
		current_x = pos.x;
		current_z = pos.z;
		std::cout << "Current Z is " << current_z << std::endl;
		std::cout << "Target Z is " << target_z << std::endl;

		//if the pos hasn't changed after 20 checks, break out of while loop
		if (current_x == last_current_x && current_z == last_current_z)
		{
			pos_not_moved_count++;
			if (pos_not_moved_count >= 20)
			{
				break;
			}
		}
		last_current_x = current_x;
		last_current_z = current_z;
		//adding this so that if we go over we don't get stuck in a loop
		if (current_z > target_z)
		{
			break;
		}
	}
	std::cout << "finished setting char position" << std::endl;
}



std::vector<flyff::monster> get_initial_monsters_on_field(const flyff::process &process, const std::array<unsigned char, 104> byte_pattern_to_match_monster_id)
{
	std::vector<flyff::monster> found_monsters{};
	found_monsters.reserve(65);

	//LPVOID bufferRead = nullptr;
	bool monster_id_found = false;
	bool break_loop_1 = false;
	bool set_possible_match = false;
	std::array<unsigned char, 4>  monster_id_bytes{};
	//monster_id_bytes.reserve(4);

	std::array<unsigned char, 104> possible_monster_id_match{};
	//possible_monster_id_match.resize(byte_pattern_to_match_monster_id.size());

	bool continue_early = false;

	std::cout << "starting initial monster search" << std::endl;

	//monster_id finder
	//std::cout << "starting monster finder " << std::endl;

	flyff::memory mem(process.queryFoundAddress, process.pHandle, process.bytesToRead);
	mem.read();

	while (!monster_id_found)
	{

		//std::cout << "Read the process memory and moved it to the buffer" << std::endl;

		//std::cout << "checking for value in the buffer" << std::endl;
		//59 bytes for the pattern that we'll compare to the memory we scan, and I marked some as 0x00 because I won't compare those

		uint64_t bufferSize = static_cast<int>(process.bytesToRead);


		//std::cout << "possible_monster_id_match size is " << byte_pattern_to_match_monster_id.size() << std::endl;
		// the monsters never seem to be over half of the address space
		// moving every 8 bytes to speed it up
		for (uint64_t byte = 0; byte < bufferSize /2; byte += 8)
		{
			if (break_loop_1)
			{
				break;
			}

			//get all of the bytes of this region's section 
			for (uint64_t b = 0; b < byte_pattern_to_match_monster_id.size(); b++)
			{
				// std::wcout << "byte in the nested foor loop is " << byte << std::endl;
				// std::wcout << "assigning b position of possible_monster_id_match at " << b << std::endl;
				if (byte + b >= 0 && byte + b < bufferSize / 2)
				{
					//std::cout << "adding bytes in possible_monster_id_match" << std::endl;
					possible_monster_id_match[b] = static_cast<unsigned char *>(mem.buffer)[byte + b];


					//byte 13 is unique so there's a its existence means we have a match. Otherwise we can move on and not take the rest of the 91 bytes.
					if (b == 13)
					{
						if (possible_monster_id_match[13] != byte_pattern_to_match_monster_id[13])
						{
							continue_early = true;
							break;
						}
					}
				}
				else
				{
					//need to break out of the inner and outer for because there are no valid monsters after
					//std::cout << "Unable to assign possible_monster_id_match in initial check at byte " << byte << std::endl;
					break_loop_1 = true;
					break;
				}
			}

			if (continue_early)
			{
				continue_early = false;
				continue;
			}

			if (byte_pattern_to_match_monster_id.size() == 104)
			{

				possible_monster_id_match[4] = 0x00;
				possible_monster_id_match[5] = 0x00;
				possible_monster_id_match[6] = 0x00;
				possible_monster_id_match[7] = 0x00;

				//possible_monster_id_match[13) = 0x00;
				//possible_monster_id_match[15) = 0x00;

				possible_monster_id_match[20] = 0x00;
				possible_monster_id_match[21] = 0x00;
				possible_monster_id_match[22] = 0x00;
				possible_monster_id_match[23] = 0x00;

				//24-27 are the monster id
				possible_monster_id_match[24] = 0x00;
				possible_monster_id_match[25] = 0x00;
				possible_monster_id_match[26] = 0x00;
				possible_monster_id_match[27] = 0x00;

				possible_monster_id_match[45] = 0x00;
				possible_monster_id_match[46] = 0x00;
				possible_monster_id_match[47] = 0x00;

				possible_monster_id_match[58] = 0x00;
				possible_monster_id_match[59] = 0x00;

				possible_monster_id_match[61] = 0x00;
				possible_monster_id_match[62] = 0x00;
				possible_monster_id_match[63] = 0x00;

				possible_monster_id_match[77] = 0x00;
				possible_monster_id_match[78] = 0x00;
				possible_monster_id_match[79] = 0x00;

				possible_monster_id_match[93] = 0x00;
				possible_monster_id_match[94] = 0x00;
				possible_monster_id_match[95] = 0x00;

				set_possible_match = true;
			}

			if (!set_possible_match)
			{
				std::cout << "The possible_monster_id_match size may be incorrect because we didn't set any" << std::endl;
				break;
			}

			//if (byte_pattern_to_match_monster_id.size() == possible_monster_id_match.size())
			//{
			//	//std::cout << "vectors are same size, testing match" << std::endl;
			//	for (int i = 0; i < byte_pattern_to_match_monster_id.size(); ++i)
			//	{
			//		if (byte_pattern_to_match_monster_id[i] != possible_monster_id_match[i])
			//		{
			//			//std::cout << "vectors are not a match" << std::endl;
			//			continue_outer_loop = true;
			//			break;

			//		}
			//	}
			//}
			//else
			//{
			//	std::cout << "the pattern and the possible match aren't the same size" << std::endl;
			//	break;
			//}

			//if (continue_outer_loop)
			//{
			//	continue;
			//}

			if (byte_pattern_to_match_monster_id != possible_monster_id_match)
			{
				continue;
			}

			//std::cout << "found bytes after z at " << byte << std::endl;
			//the float is after the 45 00 00 00 00.

			monster_id_bytes[0] = static_cast<unsigned char *>(mem.buffer)[byte + 24];
			monster_id_bytes[1] = static_cast<unsigned char *>(mem.buffer)[byte + 25];
			monster_id_bytes[2] = static_cast<unsigned char *>(mem.buffer)[byte + 26];
			monster_id_bytes[3] = static_cast<unsigned char *>(mem.buffer)[byte + 27];
			

			flyff::monster mon{};
			mon.address = (LPVOID)((uintptr_t)process.queryFoundAddress + byte + 24);
			//std::cout << "mon address is " << std::hex << mon.address << std::dec << std::endl;
			memcpy(&mon.id, monster_id_bytes.data(), sizeof(int));
			if (mon.id == 0)
			{
				continue;
			}

			//////

			std::array<unsigned char, 4> mon_z_bytes{};
			mon_z_bytes[0] = static_cast<unsigned char *>(mem.buffer)[byte - 56];
			mon_z_bytes[1] = static_cast<unsigned char *>(mem.buffer)[byte - 55];
			mon_z_bytes[2] = static_cast<unsigned char *>(mem.buffer)[byte - 54];
			mon_z_bytes[3] = static_cast<unsigned char *>(mem.buffer)[byte - 53];
			memcpy(&mon.z, &mon_z_bytes, sizeof(int));
			mon.zaddress = (LPVOID)((uintptr_t)process.queryFoundAddress + byte - 56);

			//std::cout << "mon z coord is " << mon.z << std::endl;

			std::array<unsigned char, 4> mon_x_bytes{};
			mon_x_bytes[0] = static_cast<unsigned char *>(mem.buffer)[byte - 52];
			mon_x_bytes[1] = static_cast<unsigned char *>(mem.buffer)[byte - 51];
			mon_x_bytes[2] = static_cast<unsigned char *>(mem.buffer)[byte - 50];
			mon_x_bytes[3] = static_cast<unsigned char *>(mem.buffer)[byte - 49];
			memcpy(&mon.x, &mon_x_bytes, sizeof(int));
			mon.xaddress = (LPVOID)((uintptr_t)process.queryFoundAddress + byte - 52);

			mon.distance = mon.x + mon.z;

			//std::cout << "mon x coord is " << mon.x << std::endl;


			std::array<unsigned char, 4> mon_y_bytes{};
			mon_y_bytes[0] = static_cast<unsigned char *>(mem.buffer)[byte - 48];
			mon_y_bytes[1] = static_cast<unsigned char *>(mem.buffer)[byte - 47];
			mon_y_bytes[2] = static_cast<unsigned char *>(mem.buffer)[byte - 46];
			mon_y_bytes[3] = static_cast<unsigned char *>(mem.buffer)[byte - 45];
			memcpy(&mon.y, &mon_y_bytes, sizeof(int));

			//std::cout << "found monster at base add " << process.queryFoundAddress << " and byte " << byte << std::endl;
			found_monsters.push_back(std::move(mon));
		}

		if (break_loop_1)
		{
			break;
		}
	}
	return found_monsters;
}



void flyff::monster::update_target(const flyff::process &process)
{
	if (address == nullptr)
	{
		std::cout << "monster address is nullptr" << std::endl;
		return;
	}

	//LPVOID bufferRead = nullptr;
	bool monster_id_found = false;
	bool break_loop_1 = false;
	bool continue_outer_loop = false;
	uint64_t mon_bytes_buffer_size = 2048;
	bool continue_early = false;

	flyff::memory mem(address, process.pHandle, mon_bytes_buffer_size);
	mem.read();
	//std::cout << "trying to update target id of mon " << id << " at add " << address << std::endl;

	while (!monster_id_found)
	{
		//std::cout << "starting monster finder " << std::endl;
		//allocating and reading 2048 bytes (mon_bytes_to_read) because the monster target info should be around 1.5k after the mon ID address

		std::array<unsigned char, 56> possible_monster_target_id_match{};
		//possible_monster_target_id_match.resize(monster_target_byte_pattern.size());
		// the monsters never seem to be over half of the address space
		for (uint64_t byte = 0; byte < mon_bytes_buffer_size; byte++)
		{
			if (break_loop_1)
			{
				break;
			}
			continue_outer_loop = false;

			//get all of the bytes of this region's section 
			for (uint64_t b = 0; b < monster_target_byte_pattern.size(); b++)
			{
				// std::wcout << "byte in the nested foor loop is " << byte << std::endl;
				// std::wcout << "assigning b position of possible_monster_id_match at " << b << std::endl;
				if (byte + b >= 0 && byte + b < mon_bytes_buffer_size)
				{
					//std::cout << "adding bytes in possible_monster_id_match" << std::endl;
					possible_monster_target_id_match[b] = static_cast<unsigned char *>(mem.buffer)[byte + b];
					if (b == 0)
					{
						//exit early if byte 0 doesn't match because it's unique
						if (possible_monster_target_id_match[0] != monster_target_byte_pattern[0])
						{
							continue_early = true;
							break;
						}
					}
	
				}
				else
				{
					//need to break out of the inner and outer for because there are no valid monsters after
					//std::cout << "Unable to assign possible_monster_target_id_match at byte " << byte << std::endl;
					break_loop_1 = true;
					break;
				}
			}

			if (continue_early)
			{
				continue_early = false;
				continue;
			}

			possible_monster_target_id_match[8] = 0x00;
			possible_monster_target_id_match[9] = 0x00;
			possible_monster_target_id_match[10] = 0x00;
			possible_monster_target_id_match[11] = 0x00;
			possible_monster_target_id_match[12] = 0x00;
			possible_monster_target_id_match[13] = 0x00;
			possible_monster_target_id_match[14] = 0x00;
			possible_monster_target_id_match[15] = 0x00;

			//id that the mon is following
			possible_monster_target_id_match[32] = 0x00;
			possible_monster_target_id_match[33] = 0x00;
			possible_monster_target_id_match[34] = 0x00;
			possible_monster_target_id_match[35] = 0x00;

			//target but it doesn't go away when the monster dies
			possible_monster_target_id_match[40] = 0x00;
			possible_monster_target_id_match[41] = 0x00;
			possible_monster_target_id_match[42] = 0x00;
			possible_monster_target_id_match[43] = 0x00;

			//target id and it goes away when t he monster dies
			possible_monster_target_id_match[48] = 0x00;
			possible_monster_target_id_match[49] = 0x00;
			possible_monster_target_id_match[50] = 0x00;
			possible_monster_target_id_match[51] = 0x00;


			//if (monster_target_byte_pattern.size() == possible_monster_target_id_match.size())
			//{
			//	//std::cout << "vectors are same size, testing match" << std::endl;
			//	for (int i = 0; i < monster_target_byte_pattern.size(); ++i)
			//	{
			//		if (monster_target_byte_pattern[i] != possible_monster_target_id_match[i])
			//		{
			//			//std::cout << "vectors are not a match" << std::endl;
			//			continue_outer_loop = true;
			//			break;

			//		}
			//	}
			//}
			//else
			//{
			//	std::cout << "the pattern and the possible match aren't the same size" << std::endl;
			//	break;
			//}

			//if (continue_outer_loop)
			//{
			//	continue;
			//}

			if (monster_target_byte_pattern != possible_monster_target_id_match)
			{
				continue;
			}

			//40-43 are the bytes of the monster's target, and they go away when the monster dies

			std::array<unsigned char, 4> mon_target_bytes{};
			mon_target_bytes[0] = static_cast<unsigned char *>(mem.buffer)[byte + 48];
			mon_target_bytes[1] = static_cast<unsigned char *>(mem.buffer)[byte + 49];
			mon_target_bytes[2] = static_cast<unsigned char *>(mem.buffer)[byte + 50];
			mon_target_bytes[3] = static_cast<unsigned char *>(mem.buffer)[byte + 51];
			memcpy(&target, &mon_target_bytes, sizeof(int));
			//std::cout << "monster target is " << target << std::endl;
			break_loop_1 = true;
			monster_id_found = true;
			//////
		}

		if (break_loop_1)
		{
			break;
		}
	}
}



bool flyff::monster::check_if_pet(const flyff::process &process)
{
	if (address == nullptr)
	{
		std::cout << "monster address is nullptr" << std::endl;
		return false;
	}
	bool break_loop_1 = false;
	//bool continue_early = false;
	uint64_t mon_bytes_buffer_size = 2048;

	//std::cout << "trying to update target id of mon " << id << " at add " << address << std::endl;

	flyff::memory mem(address, process.pHandle, mon_bytes_buffer_size);
	mem.read();

	//this char size must match the pattern in the class private section
	std::array<unsigned char, 8> possible_pet_match{};
	// the monsters never seem to be over half of the address space
	for (uint64_t byte = 1024; byte < mon_bytes_buffer_size; byte++)
	{
		if (break_loop_1)
		{
			break;
		}

		//get all of the bytes of this region's section 
		for (uint64_t b = 0; b < pet_byte_pattern.size(); b++)
		{
			// std::wcout << "byte in the nested foor loop is " << byte << std::endl;
			// std::wcout << "assigning b position of possible_monster_id_match at " << b << std::endl;
			if (byte + b >= 0 && byte + b < mon_bytes_buffer_size)
			{
				//std::cout << "adding bytes in possible_monster_id_match" << std::endl;
				possible_pet_match[b] = static_cast<unsigned char *>(mem.buffer)[byte + b];
				////since the first byte is unique, we can check if it exists before getting the rest
				////exit early if the first byte doesn't match
				//if (b == 0)
				//{
				//	if (possible_pet_match[0] != pet_byte_pattern[0])
				//	{
				//		continue_early = true;
				//		break;
				//	}
				//}

			}
			else
			{
				//need to break out of the inner and outer for because there are no valid monsters after
				//std::cout << "Unable to assign possible_pet_match at byte " << byte << std::endl;
				break_loop_1 = true;
				break;
			}
		}
	/*	if (continue_early)
		{
			continue_early = false;
			break;
		}*/
		////purely for reference
		//FF FF FF FF
		//00 00 00 00 00 00 00 00
		//00 00 00 00 E1 7A 94 3F << pattern
		////end reference


		if (pet_byte_pattern.size() == possible_pet_match.size())
		{

			if (pet_byte_pattern == possible_pet_match)
			{
				std::cout << "found the pet pattern for mon " << id << std::endl;
				return true;
			}
			else
			{
				continue;
			}
		}
		else
		{
			std::cout << "the pattern and the possible match aren't the same size" << std::endl;
			break;
		}
	}
	return false;
}


void flyff::player::update_target()
{
	size_t bytesToRead = 128;
	std::array<unsigned char, 4> player_target_bytes{};
	std::cout << "starting get precise player target" << std::endl;
	flyff::memory mem(player_target_proc.player_target_address, player_target_proc.pHandle, bytesToRead);
	mem.read();
	player_target_bytes[0] = static_cast<unsigned char *>(mem.buffer)[0];
	player_target_bytes[1] = static_cast<unsigned char *>(mem.buffer)[1];
	player_target_bytes[2] = static_cast<unsigned char *>(mem.buffer)[2];
	player_target_bytes[3] = static_cast<unsigned char *>(mem.buffer)[3];
	memcpy(&target, &player_target_bytes, sizeof(int));

	std::cout << "player target is " << target << std::endl;
	if (log_file_->log_level_is_verbose())
	{
		log_write("player target is " + std::to_string(target));
	}
}


void flyff::player::update_combat_status()
{
	std::array<unsigned char, 4> player_target_bytes{};

	std::cout << "starting get_player_combat_status" << std::endl;

	flyff::memory mem(player_target_proc.player_target_address, player_target_proc.pHandle, 128);
	mem.read();

	player_target_bytes[0] = static_cast<unsigned char *>(mem.buffer)[8];
	player_target_bytes[1] = static_cast<unsigned char *>(mem.buffer)[9];
	player_target_bytes[2] = static_cast<unsigned char *>(mem.buffer)[10];
	player_target_bytes[3] = static_cast<unsigned char *>(mem.buffer)[11];


	int combat_monster = 0;
	memcpy(&combat_monster, &player_target_bytes, sizeof(int));
	if (combat_monster == 0)
	{
		combat = false;
		if (log_file_->log_level_is_verbose())
		{
			std::cout << "player is NOT in combat" << std::endl;
			if (log_file_->log_level_is_verbose())
			{
				log_write("player is NOT in combat");
			}
		}
	}
	else
	{
		combat = true;
		std::cout << "player is in combat" << std::endl;
		if (log_file_->log_level_is_verbose())
		{
			log_write("player is in combat");
		}
	}
}


void flyff::player::set_target(int target_id)
{
	unsigned char data[4]{};
	flyff::utils::int_to_hex(target_id, data);

	if (!WriteProcessMemory(player_target_proc.pHandle, player_target_proc.player_target_address, &data, sizeof(int), NULL))
	{
		std::cout << "ERROR: Unable to write process mem for set_target" << std::endl;
		log_write("ERROR: Unable to write process mem for set_target");
		throw std::runtime_error("ERROR: Unable to write process mem for set_target");
		

	}
}


void sort_and_filter_monsters_by_distance(const char_position &pos, std::vector<flyff::monster> &initial_monsters_on_field)
{

	for (flyff::monster &mon : initial_monsters_on_field)
	{
		mon.order = sqrt(pow((mon.x - pos.x), 2) + pow((mon.z - pos.z), 2));
	}

	std::sort(initial_monsters_on_field.begin(), initial_monsters_on_field.end(), [](const flyff::monster &a, const flyff::monster &b)
		{
			return a.order > b.order;
		});

}




void flyff::monster::update_pos(const flyff::process &process)
{
	//uint64_t mon_bytes_buffer_size{ 48 };

	flyff::memory mem(zaddress, process.pHandle, 48);
	mem.read();

	std::array<unsigned char, 4> z_bytes{};
	z_bytes[0] = static_cast<unsigned char *>(mem.buffer)[0];
	z_bytes[1] = static_cast<unsigned char *>(mem.buffer)[1];
	z_bytes[2] = static_cast<unsigned char *>(mem.buffer)[2];
	z_bytes[3] = static_cast<unsigned char *>(mem.buffer)[3];
	memcpy(&z, &z_bytes, sizeof(int));
	//std::cout << "monster Z pos is " << monster.z << std::endl;
	////
	std::array<unsigned char, 4> x_bytes{};
	x_bytes[0] = static_cast<unsigned char *>(mem.buffer)[4];
	x_bytes[1] = static_cast<unsigned char *>(mem.buffer)[5];
	x_bytes[2] = static_cast<unsigned char *>(mem.buffer)[6];
	x_bytes[3] = static_cast<unsigned char *>(mem.buffer)[7];
	memcpy(&x, &x_bytes, sizeof(int));
	//std::cout << "monster X pos is " << monster.x << std::endl;

	std::array<unsigned char, 4> y_bytes{};
	y_bytes[0] = static_cast<unsigned char *>(mem.buffer)[8];
	y_bytes[1] = static_cast<unsigned char *>(mem.buffer)[9];
	y_bytes[2] = static_cast<unsigned char *>(mem.buffer)[10];
	y_bytes[3] = static_cast<unsigned char *>(mem.buffer)[11];
	memcpy(&y, &y_bytes, sizeof(int));

	if (x == 0 || z == 0)
	{
		x = 0;
		z = 0;
	}
}


bool flyff::monster::check_if_valid(const flyff::process &process)
{

	flyff::memory mem(address, process.pHandle, 8);
	mem.read();

	std::array<unsigned char, 4> id_bytes{};
	id_bytes[0] = static_cast<unsigned char *>(mem.buffer)[0];
	id_bytes[1] = static_cast<unsigned char *>(mem.buffer)[1];
	id_bytes[2] = static_cast<unsigned char *>(mem.buffer)[2];
	id_bytes[3] = static_cast<unsigned char *>(mem.buffer)[3];
	int id_to_check = 0;
	memcpy(&id_to_check, &id_bytes, sizeof(int));
	if (id == id_to_check)
	{
		valid_ = true;
	}
	else
	{
		valid_ = false;
	}
	return valid_;
	//std::cout << "updated pos of mon " << id << " : x " << x << " : y " << y << " : z " << z << std::endl;
}



void flyff::player::setup_initial_target(const flyff::process &process)
{
	LPVOID queryBaseAddress = process.queryFoundAddress;
	size_t bytesToRead = process.bytesToRead;
	bool player_target_found = false;
	std::array<unsigned char, 4> player_target_bytes{};
	//this pHandle gets passed to the precise func later
	player_target_proc.pHandle = process.pHandle;
	bool break_loop_1 = false;
	bool continue_outer_loop = false;
	int player_name_chars_matched = 0;
	bool break_all_loops = false;
	bool break_player_name_loop = false;
	//resizing vectors should be outside of the while loop because the ops run new/delete quite a bit
	std::array<unsigned char, 136> possible_player_target_match{};
	bool found_player_name = false;
	std::vector<char> possible_player_name{};
	//possible_player_name.reserve(name.size());
	possible_player_name.resize(name.size());
	bool continue_early = false;

	flyff::memory mem(process.queryFoundAddress, player_target_proc.pHandle, bytesToRead);
	mem.read();

	while (!player_target_found)
	{
		if (break_all_loops) { break; }

		std::cout << "starting initial player target finder " << std::endl;

		//std::cout << "Read the process memory and moved it to the buffer" << std::endl;

		//std::cout << "checking for value in the buffer" << std::endl;
		//59 bytes for the pattern that we'll compare to the memory we scan, and I marked some as 0x00 because I won't compare those

		uint64_t bufferSize = static_cast<int>(bytesToRead);
		//Thise whole for look pulls out 59 bytes of memory, replaces the masked values, and compares to our original pattern for a monster ID match
		//go through each byte in the buffer bufferRead by counting bytesToRead

		//std::cout << "player_target_byte_pattern size is " << player_target_byte_pattern.size() << std::endl;
		//every 4 bytes was the sweet spot for finding this quickly
		//the data never seems to be before 4 mil
		for (uint64_t byte = 2'000'000; byte < bufferSize; byte += 4)
		{
			//outside_byte_tracker = byte;
			//if (((uintptr_t)queryBaseAddress + (uint64_t)byte) == 0x2ED80B73787)
			//{
			//	std::cout << "close to the add" << std::endl;
			//}

			if (break_loop_1)
			{
				break;
			}
			continue_outer_loop = false;

			//get all of the bytes of this region's section 
			for (uint64_t b = 0; b < player_target_byte_pattern.size(); ++b)
			{
				// std::wcout << "byte in the nested foor loop is " << byte << std::endl;
				// std::wcout << "assigning b position of possible_player_target_match at " << b << std::endl;
				if (byte + b < bufferSize)
				{
					//std::cout << "adding bytes in possible_player_target_match" << std::endl;
					possible_player_target_match[b] = static_cast<unsigned char *>(mem.buffer)[byte + b];
					if (b == 13)
					{
						if (possible_player_target_match[13] != player_target_byte_pattern[13])
						{
							continue_early = true;
							break;
						}
					}
				}
				else
				{
					//need to break out of the inner and outer for because there are no valid monsters after
					std::cout << "Unable to assign possible_player_target_match in initial check at byte " << byte << std::endl;
					break_loop_1 = true;
					break;
				}
			}

			if (continue_early)
			{
				continue_early = false;
				continue;
			}

			//at this point we've collected the possible pattern from mem and we need to replace the masked values before comparing
			//
			//in v3 24-31  random (this is from pattern 3 match, found after healer wasn't found but fighter was
			// I added 8 to the front and back for a sliding window match


			possible_player_target_match[0] = 0x00;
			possible_player_target_match[1] = 0x00;
			possible_player_target_match[2] = 0x00;
			possible_player_target_match[3] = 0x00;
			possible_player_target_match[4] = 0x00;
			possible_player_target_match[5] = 0x00;
			possible_player_target_match[6] = 0x00;
			possible_player_target_match[7] = 0x00;


			possible_player_target_match[16] = 0x00;
			possible_player_target_match[17] = 0x00;
			possible_player_target_match[18] = 0x00;
			possible_player_target_match[19] = 0x00;
			possible_player_target_match[20] = 0x00;
			possible_player_target_match[21] = 0x00;
			possible_player_target_match[22] = 0x00;
			possible_player_target_match[23] = 0x00;

			//32-39 random data
			possible_player_target_match[32] = 0x00;
			possible_player_target_match[33] = 0x00;
			possible_player_target_match[34] = 0x00;
			possible_player_target_match[35] = 0x00;
			possible_player_target_match[36] = 0x00;
			possible_player_target_match[37] = 0x00;
			possible_player_target_match[38] = 0x00;
			possible_player_target_match[39] = 0x00;

			//40-43 random data
			possible_player_target_match[40] = 0x00;
			possible_player_target_match[41] = 0x00;
			possible_player_target_match[42] = 0x00;
			possible_player_target_match[43] = 0x00;

			//48-51 random data
			possible_player_target_match[48] = 0x00;
			possible_player_target_match[49] = 0x00;
			possible_player_target_match[50] = 0x00;
			possible_player_target_match[51] = 0x00;

			//player target is 56-59
			possible_player_target_match[56] = 0x00;
			possible_player_target_match[57] = 0x00;
			possible_player_target_match[58] = 0x00;
			possible_player_target_match[59] = 0x00;

			//in v3 64-67 are player target again, but we'll treat as random data (player target shows up twice]
			possible_player_target_match[64] = 0x00;
			possible_player_target_match[65] = 0x00;
			possible_player_target_match[66] = 0x00;
			possible_player_target_match[67] = 0x00;

			//72-75 are the char followed, but we'll treat as random data
			possible_player_target_match[72] = 0x00;
			possible_player_target_match[73] = 0x00;
			possible_player_target_match[74] = 0x00;
			possible_player_target_match[75] = 0x00;

			//in v3 random
			possible_player_target_match[80] = 0x00;
			possible_player_target_match[81] = 0x00;
			possible_player_target_match[82] = 0x00;
			possible_player_target_match[83] = 0x00;

			//v6, 108 random
			possible_player_target_match[108] = 0x00;

			//v5, 112, 120 - 121, 124, 126 - 127 random
			possible_player_target_match[112] = 0x00;

			//116 seems to be some type of combat
			possible_player_target_match[116] = 0x00;


			possible_player_target_match[120] = 0x00;
			possible_player_target_match[121] = 0x00;
			possible_player_target_match[124] = 0x00;
			possible_player_target_match[126] = 0x00;
			possible_player_target_match[127] = 0x00;

			possible_player_target_match[128] = 0x00;
			possible_player_target_match[129] = 0x00;
			possible_player_target_match[130] = 0x00;
			possible_player_target_match[131] = 0x00;
			possible_player_target_match[132] = 0x00;
			possible_player_target_match[133] = 0x00;
			possible_player_target_match[134] = 0x00;
			possible_player_target_match[135] = 0x00;

	
			if (possible_player_target_match != player_target_byte_pattern)
			{
				continue_outer_loop = true;
			}

			//if (player_target_byte_pattern.size() == possible_player_target_match.size())
			//{
			//	//std::cout << "vectors are same size, testing match" << std::endl;
			//	for (int i = 0; i < player_target_byte_pattern.size(); ++i)
			//	{
			//		if (player_target_byte_pattern[i] != possible_player_target_match[i])
			//		{
			//			//std::cout << "vectors are not a match" << std::endl;
			//			continue_outer_loop = true;
			//			break;
			//		}
			//	}
			//}
			//else
			//{
			//	std::cout << "the pattern and the possible match aren't the same size" << std::endl;
			//	break;
			//}
			
			if (continue_outer_loop)
			{
				continue;
			}

			//std::cout << "found the player target match pattern, checking for player name" << std::endl;

			//std::cout << "at add " << std::hex << (uintptr_t)queryBaseAddress + (uint64_t)byte  << std::endl;

			//this is a 3 level for loop that scans bytes 600-1000 AFTER finding the player match pattern.
			//We loop for our player name because there is a match for every player on the field.
			//The location of the player name ranges somewhere in the 700 bye range, so I am using 600-1000 to be safe

			for (uint64_t b1 = 100; b1 < 1000; b1++)
			{

				for (uint64_t i = 0; i < possible_player_name.size(); i++)
				{
					//std::cout << "char " << static_cast<unsigned char *>(bufferRead)[byte + 48 + b1 + i] << std::endl;
					possible_player_name[i] = static_cast<unsigned char *>(mem.buffer)[byte + b1 + i];
					//std::cout << possible_player_name[i);
				}

				player_name_chars_matched = 0;

				//If all chars between our player name and possible player name match,  break the outer loop and continue processing.
				//If we don't find a match for the player name, we keep taking X byte chunks between 600-1000 bytes after the player target pattern
				for (uint64_t i = 0; i < name.size(); i++)
				{

					if (name[i] != possible_player_name[i])
					{
						//std::cout << "player name not in pattern" << std::endl;
						break_player_name_loop = false;
						break;
					}
					else //this means we found a match for the player name, so we no longer need to keep scanning
					{
						player_name_chars_matched++;
						if (player_name_chars_matched == name.size())
						{
							break_player_name_loop = true;
							found_player_name = true;
							player_name_chars_matched = 0;
							break;
						}

					}

				}
				if (break_player_name_loop)
				{
					break;
				}
			}

			//needed to continue finding the correct player target with my chosen player name, or else we leave the function too early
			if (continue_outer_loop)
			{
				continue;
			}

			// we didn't find the player name in the target pattern, restart the scan
			if (!found_player_name)
			{
				continue;
			}

			std::cout << "found the player name " << name << " in the target" << std::endl;

			//If we made it to this point, it means we found a match and we should grab the bytes and convert it into an int
			player_target_bytes[0] = static_cast<unsigned char *>(mem.buffer)[byte + 56];
			player_target_bytes[1] = static_cast<unsigned char *>(mem.buffer)[byte + 57];
			player_target_bytes[2] = static_cast<unsigned char *>(mem.buffer)[byte + 58];
			player_target_bytes[3] = static_cast<unsigned char *>(mem.buffer)[byte + 59];

			memcpy(&target, &player_target_bytes, sizeof(int));
			std::cout << "player target is " << target << std::endl;

			//The pattern address is passed to the precise function to quickly get the value in subsequent runs
			player_target_proc.player_target_pattern_address = (LPVOID)((uintptr_t)queryBaseAddress + (uint64_t)byte);
			player_target_proc.player_target_address = (LPVOID)((uintptr_t)queryBaseAddress + (uint64_t)byte + 56);
			player_target_proc.bytesToRead = 1024;

			//We need to do a similar search for our player ID, not sure how
			std::array<unsigned char, 4> player_id_bytes{};
			player_id_bytes[0] = static_cast<unsigned char *>(mem.buffer)[byte + 56 - 1504];
			player_id_bytes[1] = static_cast<unsigned char *>(mem.buffer)[byte + 56 - 1503];
			player_id_bytes[2] = static_cast<unsigned char *>(mem.buffer)[byte + 56 - 1502];
			player_id_bytes[3] = static_cast<unsigned char *>(mem.buffer)[byte + 56 - 1501];
			memcpy(&id, &player_id_bytes, sizeof(int));

			std::cout << "player ID is " << id << std::endl;
			//we don't really use the ID yet, but maybe later we will
			player_target_proc.player_id_address = (LPVOID)((uintptr_t)queryBaseAddress + (uint64_t)byte + 56 - 1504);


			player_target_found = true;
			break;
		}

		std::cout << " player target address is " << player_target_proc.player_target_address << std::endl;

		if (break_loop_1)
		{
			break;
		}
	}
}



void flyff::player::set_log_file(flyff::log *log_file)
{
	log_file_ = log_file;
	log_set_ = true;
}



void flyff::player::log_write(const std::string &msg)
{
	if (log_file_ != nullptr)
	{
		log_file_->write(msg);
	}
}

void flyff::monster::set_log_file(flyff::log *log_file)
{
	log_file_ = log_file;
	log_set_ = true;
}

void flyff::monster::log_write(const std::string &msg)
{
	if (log_file_ != nullptr)
	{
		log_file_->write(msg);
	}
}

std::vector<LPVOID> flyff::player::find_all_text_targets(const flyff::process &process)
{
	LPVOID queryBaseAddress = process.queryFoundAddress;
	size_t queryFoundRegionSize = process.bytesToRead;
	size_t bytesToRead = process.bytesToRead;
	//LPVOID bufferRead = nullptr;
	bool player_target_found = false;
	bool break_loop_1 = false;
	bool continue_outer_loop = false;
	int player_name_chars_matched = 0;
	bool break_all_loops = false;
	bool break_player_name_loop = false;

	player_name_addresses_.reserve(8);

	uint64_t outside_byte_tracker = 0;

	std::vector<unsigned char> player_target_text{};
	player_target_text.resize(name.size());

	for (int x = 0; x < player_target_text.size(); ++x)
	{
		player_target_text[x] = name[x];
	}

	std::vector<unsigned char> possible_player_target_text{};
	possible_player_target_text.resize(name.size());

	bool found_player_name = false;
	bool continue_early = false;


	//std::cout << "starting setup_initial_text_target " << std::endl;
	if (log_file_->log_level_is_verbose())
	{
		log_write("starting setup_initial_text_target");

	}

	flyff::memory mem(process.queryFoundAddress, player_target_proc.pHandle, bytesToRead);
	mem.read();

		uint64_t bufferSize = static_cast<int>(bytesToRead);

		for (uint64_t byte = 0; byte < bufferSize; ++byte)
		{
	
			if (break_loop_1)
			{
				break;
			}
			continue_outer_loop = false;

			for (uint64_t b = 0; b < player_target_text.size(); ++b)
			{

				if (byte + b < bufferSize)
				{
					possible_player_target_text[b] = static_cast<unsigned char *>(mem.buffer)[byte + b];
					//do an early comparison on the first char
					if (b == 0)
					{
						if (possible_player_target_text[0] != player_target_text[0])
						{
							continue_early = true;
							break;
						}
					}
				}
				else
				{
					break_loop_1 = true;
					break;
				}
			}

			//if we broke out of the loop early move to the next byte
			if (continue_early)
			{
				continue_early = false;
				continue;
			}

			if (player_target_text.size() == possible_player_target_text.size())
			{
				//std::cout << "vectors are same size, testing match" << std::endl;
				for (int i = 0; i < player_target_text.size(); ++i)
				{
					if (player_target_text[i] == possible_player_target_text[i])
					{
						if (i + 1 == player_target_text.size())
						{
							found_player_name = true;
						}
					}
					else
					{
						//std::cout << "vectors are not a match" << std::endl;
						continue_outer_loop = true;
						break;
					}
				}
			}
			else
			{
				log_write("ERROR: The pattern and the possible match aren't the same size in the setup_initial_text_target");
				std::cout << "the pattern and the possible match aren't the same size in the setup_initial_text_target" << std::endl;
				throw std::runtime_error("ERROR: The pattern and the possible match aren't the same size in the setup_initial_text_target");
			}
			if (continue_outer_loop)
			{
				continue;
			}

			if (found_player_name)
			{
				if (log_file_->log_level_is_verbose())
				{
					log_write("found the player name " + name + " at address " + std::to_string((uintptr_t)queryBaseAddress + (uint64_t)byte));
				}
				std::cout << "found the player name " << name << " at address " << (LPVOID)((uintptr_t)queryBaseAddress + (uint64_t)byte) << std::endl;
				player_name_addresses_.push_back((LPVOID)((uintptr_t)queryBaseAddress + (uint64_t)byte));
			}
		}
		return player_name_addresses_;
}


bool flyff::player::check_if_text_target_is_bad_mon(std::vector<LPVOID> &all_text_targets)
{
	for (LPVOID &add_to_check : all_text_targets)
	{
		//log_write("checking address " + std::to_string(reinterpret_cast<uintptr_t>(add_to_check)) + " for giant/violet/baby/eron text");
		size_t bytesToRead = 12;
		//LPVOID bufferRead = nullptr;

		std::array<unsigned char, 4> giant_target_text = { 'G','i','a','n' };
		std::array<unsigned char, 4> violet_target_text = { 'V','i','o','l' };
		std::array<unsigned char, 4> baby_target_text = { 'B','a','b','y' };
		std::array<unsigned char, 4> eron_target_text = { 'E','r','o','n' };
		std::array<unsigned char, 4> guru_target_text = { 'G','u','r','u' };
		std::array<unsigned char, 4> mine_target_text = { 'M','i','n','e' };

		std::array<unsigned char, 4> possible_bad_mon_target_text{};

		//std::cout << "starting check_if_text_target_is_bad_mon " << std::endl;

		flyff::memory mem(add_to_check, player_target_proc.pHandle, bytesToRead);
		mem.read();


		for (uint64_t b = 0; b < 4; ++b)
		{
			possible_bad_mon_target_text[b] = static_cast<unsigned char *>(mem.buffer)[b];
		}

		if (possible_bad_mon_target_text == giant_target_text ||
			possible_bad_mon_target_text == violet_target_text ||
			possible_bad_mon_target_text == baby_target_text ||
			possible_bad_mon_target_text == eron_target_text ||
			possible_bad_mon_target_text == guru_target_text ||
			possible_bad_mon_target_text == mine_target_text)
		{
			std::cout << "Found bad mon target text" << std::endl;
			if (log_file_->log_level_is_verbose())
			{
				log_write("Found bad mon target text");
			}
			return true;
		}
		else
		{
			std::cout << "Did not find giant or violet text for this monster" << std::endl;
			if (log_file_->log_level_is_verbose())
			{
				log_write("Did not find giant or violet text for this monster");
			}
			return false;
		}
	}
}


LPVOID flyff::player::check_text_targets(const flyff::process &process, LPVOID add_to_check)
{
	LPVOID queryBaseAddress = process.queryFoundAddress;
	size_t queryFoundRegionSize = process.bytesToRead;
	size_t bytesToRead = 64;
	//LPVOID bufferRead = nullptr;

	std::vector<unsigned char> player_target_text{};
	player_target_text.resize(name.size());

	for (int x = 0; x < player_target_text.size(); ++x)
	{
		player_target_text[x] = name[x];
	}

	std::vector<unsigned char> possible_player_target_text{};
	possible_player_target_text.resize(name.size());

	bool found_player_name = false;

	std::cout << "starting setup_initial_text_target " << std::endl;


	flyff::memory mem(add_to_check, player_target_proc.pHandle, bytesToRead);
	mem.read();

	for (uint64_t b = 0; b < player_target_text.size(); ++b)
	{
		possible_player_target_text[b] = static_cast<unsigned char *>(mem.buffer)[b];
	}

		if (player_target_text.size() == possible_player_target_text.size())
		{
			//std::cout << "vectors are same size, testing match" << std::endl;
			for (int i = 0; i < player_target_text.size(); ++i)
			{
				if (player_target_text[i] == possible_player_target_text[i])
				{
					if (i + 1 == player_target_text.size())
					{
						found_player_name = true;
					}
				}
				else
				{
					//std::cout << "vectors are not a match" << std::endl;
					break;
				}
			}
		}
		if (found_player_name)
		{
			std::cout << "found the player name, can't be the address" << std::endl;
			return nullptr;
		}
		else
		{
			std::cout << "found player target text address " << add_to_check << std::endl;
			return add_to_check;
		}
}

void remove_monster_from_field(int id, std::vector<flyff::monster> &initial_monsters_on_field)
{
	std::erase_if(initial_monsters_on_field, [id](const flyff::monster &x) {return  x.id == id; });
}


void delete_monster_from_field(std::vector<flyff::monster> &initial_monsters_on_field, const flyff::monster &mon_to_attack, bool mon_set)
{
	if (mon_set)
	{
		bool count_found = false;
		int ext_count = 0;
		for (int count = 0; count < initial_monsters_on_field.size(); ++count)
		{
			if (initial_monsters_on_field[count].id == mon_to_attack.id)
			{
				count_found = true;
				ext_count = count;
				break;
			}
		}
		if (count_found)
		{
			initial_monsters_on_field.erase(initial_monsters_on_field.begin() + ext_count);
		}
	}
	else
	{
		if (initial_monsters_on_field.size() > 1)
		{
			initial_monsters_on_field.pop_back();
		}
	}
}