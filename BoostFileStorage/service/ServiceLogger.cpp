#include "ServiceLogger.h"

namespace boost_file_storage
{
	ServiceLogger::ServiceLogger() : m_hEventLog(NULL)
	{ }

	ServiceLogger::~ServiceLogger()
	{
		if (m_hEventLog != NULL)
		{
			DeregisterEventSource(m_hEventLog);
		}
	}

	void ServiceLogger::log_info(const std::string &message)
	{
		log(message.c_str(), EVENTLOG_INFORMATION_TYPE);
	}

	void ServiceLogger::log_error(const std::string &message)
	{
		log(message.c_str(), EVENTLOG_ERROR_TYPE);
	}

	void ServiceLogger::log_warning(const std::string &message)
	{
		log(message.c_str(), EVENTLOG_WARNING_TYPE);
	}

	VOID ServiceLogger::log(LPCTSTR lpctsMessage, WORD wMessageType)
	{
		ReportEvent(m_hEventLog, wMessageType, 0, 0, NULL, 1, 0, &lpctsMessage, NULL);
	}

	BOOL ServiceLogger::Initialize(LPCSTR lpUNCServerName, LPCSTR lpSourceName)
	{
		m_hEventLog = RegisterEventSource(lpUNCServerName, lpSourceName);
		return IsInitialized();
	}

	BOOL ServiceLogger::IsInitialized()
	{
		return m_hEventLog != NULL;
	}
}
