#pragma once

#include "../common/logger.h"
#include <Windows.h>

namespace boost_file_storage
{
	class ServiceLogger : public logger
	{
	public:
		ServiceLogger(HANDLE hEventLog);
		~ServiceLogger();
		virtual void log(const std::string &message);
	};
}
