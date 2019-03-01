#pragma once

#include <string>

namespace boost_file_storage
{
	class logger
	{
		virtual void log(std::string message) = 0;
	};
}
