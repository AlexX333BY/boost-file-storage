#pragma once

#include <string>

namespace boost_file_storage
{
	class logger
	{
	public:
		virtual void log_info(const std::string &message) = 0;
		virtual void log_error(const std::string &message) = 0;
		virtual void log_warning(const std::string &message) = 0;
	};
}
