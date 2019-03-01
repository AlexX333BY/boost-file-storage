#pragma once

#include <string>

namespace boost_file_storage
{
	class logger
	{
	public:
		virtual void log(const std::string &message) = 0;
	};
}
