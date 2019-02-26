#include "ServiceArguments.h"
#include <climits>

namespace boost_file_storage
{
	ServiceArguments::ServiceArguments() : m_sFileSize(0), m_ucThreadCount(0), m_usListenPort(0)
	{ }

	BOOL ServiceArguments::SetData(DWORD argc, LPTSTR *argv)
	{
		if ((argc == GetRequiredArgumentsCount()) && (argv != NULL))
		{
			char **endptr;
			unsigned long long ullTemp;
			
			endptr = nullptr;
			ullTemp = strtoull(argv[1], endptr, 0);
			if ((ullTemp >= 1024) && (ullTemp <= USHRT_MAX))
			{
				SetListenPort((unsigned short)ullTemp);
			}
			else
			{
				return FALSE;
			}

			SetDownloadFolder(new std::string(argv[2]));

			endptr = nullptr;
			ullTemp = strtoull(argv[3], endptr, 0);
			if ((size_t)ullTemp != 0)
			{
				SetMaxFileSize((size_t)ullTemp);
			}
			else
			{
				return FALSE;
			}

			endptr = nullptr;
			ullTemp = strtoull(argv[4], endptr, 0);
			if ((ullTemp > 0) && (ullTemp <= UCHAR_MAX))
			{
				SetSimultaneousDownloadCount((unsigned char)ullTemp);
			}
			else
			{
				return FALSE;
			}

			return TRUE;
		}
		else
		{
			return FALSE;
		}

	}

	VOID ServiceArguments::SetDownloadFolder(const std::string *sDownloadFolder)
	{
		m_sDownloadFolder = sDownloadFolder;
	}

	const std::string *ServiceArguments::GetDownloadFolder() const
	{
		return m_sDownloadFolder;
	}

	VOID ServiceArguments::SetListenPort(unsigned short usPort)
	{
		m_usListenPort = usPort;
	}

	unsigned short ServiceArguments::GetListenPort() const
	{
		return m_usListenPort;
	}

	VOID ServiceArguments::SetSimultaneousDownloadCount(unsigned char ucThreadCount)
	{
		m_ucThreadCount = ucThreadCount;
	}

	unsigned char ServiceArguments::GetSimultaneousDownloadCount() const
	{
		return m_ucThreadCount;
	}

	VOID ServiceArguments::SetMaxFileSize(size_t sFileSize)
	{
		m_sFileSize = sFileSize;
	}

	size_t ServiceArguments::GetMaxFileSize() const
	{
		return m_sFileSize;
	}

	BYTE ServiceArguments::GetRequiredArgumentsCount()
	{
		return 5;
	}
}
