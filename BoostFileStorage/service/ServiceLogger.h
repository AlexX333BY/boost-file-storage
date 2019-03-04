#pragma once

#include "../common/logger.h"
#include <Windows.h>

namespace boost_file_storage
{
	class ServiceLogger : public logger
	{
	public:
		ServiceLogger();
		~ServiceLogger();
		virtual void log_info(const std::string &message);
		virtual void log_error(const std::string &message);
		virtual void log_warning(const std::string &message);
		BOOL Initialize(LPCSTR lpUNCServerName, LPCSTR lpSourceName);
		BOOL IsInitialized();
	protected:
		HANDLE m_hEventLog;
		VOID log(LPCTSTR lpctsMessage, WORD wMessageType);
	};
}
