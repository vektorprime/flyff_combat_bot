#pragma once

#include <iostream>
#include <fstream>

namespace flyff
{


	class log
	{
		public:

			bool log_level_is_verbose();

			log() = default;
			log(std::string file_name);
				//:
			//	file_name_(file_name),
			//	log_level_(log_level_)
			
			//log(const log &other)  :
			//	file_name_(other.file_name_),
			//	log_level_(other.log_level_),
			//	log_file_(other.log_file_)
			//{

			//}

			void set_verbose_log_level();
			
			log(log &&other) noexcept :
				file_name_(std::move(other.file_name_)),
				log_file_(std::move(other.log_file_)),
				log_level_(std::move(other.log_level_))
			{}

			log &operator=(log &&other) noexcept
			{
				if (this != &other)
				{
					file_name_ = std::move(other.file_name_);
					log_file_ = std::move(other.log_file_);
					log_level_ = std::move(other.log_level_);
				}
				return *this;
			}

			~log()
			{
				if (log_file_.is_open())
				{
					log_file_ << "closing log file \n";
					log_file_.flush();
					log_file_.close();
				}
			}

			void write(const std::string &message);
		private:

			enum class level
			{
				error,
				verbose
			};

			level log_level_ = level::error;

			std::string file_name_;
			std::ofstream log_file_;

	};
}