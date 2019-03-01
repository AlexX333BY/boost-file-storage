#pragma once
#include <string>
#include <Windows.h>

namespace boost_file_storage
{
	class ServiceArguments
	{
	public:
		ServiceArguments();
		~ServiceArguments();
		BOOL SetData(DWORD dwArgc, LPTSTR *lpszArgv);

		VOID SetDownloadFolder(const std::string *sDownloadFolder);
		const std::string *GetDownloadFolder() const;
		VOID SetListenPort(unsigned short usPort);
		unsigned short GetListenPort() const;
		VOID SetSimultaneousDownloadCount(unsigned char ucThreadCount);
		unsigned char GetSimultaneousDownloadCount() const;
		VOID SetMaxFileSize(size_t sFileSize);
		size_t GetMaxFileSize() const;

		static BYTE GetRequiredArgumentsCount();
	protected:
		const std::string *m_sDownloadFolder;
		unsigned short m_usListenPort;
		unsigned char m_ucThreadCount;
		size_t m_sFileSize;
	};
}
