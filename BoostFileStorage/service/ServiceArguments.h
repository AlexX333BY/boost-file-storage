#pragma once
#include <string>
#include <Windows.h>

namespace boost_file_storage
{
	class ServiceArguments
	{
	public:
		ServiceArguments();
		BOOL SetData(DWORD dwArgc, LPTSTR *lpszArgv);

		VOID SetDownloadFolder(std::string sDownloadFolder);
		std::string GetDownloadFolder();
		VOID SetListenPort(unsigned short usPort);
		unsigned short GetListenPort();
		VOID SetSimultaneousDownloadCount(unsigned char ucThreadCount);
		unsigned char GetSimultaneousDownloadCount();
		VOID SetMaxFileSize(size_t sFileSize);
		size_t GetMaxFileSize();

		static BYTE GetRequiredArgumentsCount();
	protected:
		std::string m_sDownloadFolder;
		unsigned short m_usListenPort;
		unsigned char m_ucThreadCount;
		size_t m_sFileSize;
	};
}
