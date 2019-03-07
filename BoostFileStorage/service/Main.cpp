#ifdef _DEBUG 
#include "../server/server.h"
#include "../common/console_logger.h"
#include <string>
#include "ServiceLogger.h"

using namespace boost_file_storage;

int main()
{
	const unsigned short dbg_port = 8080;
	const std::string dbg_download_dir = "C:\\Users\\user\\Downloads";
	const size_t dbg_max_file_size = 65536;
	const unsigned char dbg_threads = 1;

	server *s = new server(std::make_unique<console_logger>("Server"));
	if (s->initialize(dbg_port, dbg_download_dir, dbg_max_file_size, dbg_threads))
	{
		if (s->start())
		{
			system("pause");
			s->stop();
		}
	}
		
	delete s;
	system("pause");
	return 0;
}
#else
#include <stdio.h>
#include "service.h"
#include "ServiceArguments.h"
#include <Windows.h>
#include <strsafe.h>
#include "BoostFileStorageEventProvider.h"

const LPCTSTR lpcsInstallArgument = "-i";

LPTSTR GetFullRegistryKeyPath()
{
	const LPCTSTR lpcsKeyDir = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\";
	const int iNameLength = lstrlen(EVENT_PROVIDER_NAME), iDirLength = lstrlen(lpcsKeyDir), iPathLength = iNameLength + iDirLength;
	const LPTSTR lpcsKeyPath = new TCHAR[iPathLength + 1];
	lpcsKeyPath[0] = '\0';
	strcat_s(lpcsKeyPath, iPathLength + 1, lpcsKeyDir);
	strcat_s(lpcsKeyPath, iPathLength + 1, EVENT_PROVIDER_NAME);
	return lpcsKeyPath;
}

LPTSTR GetFullFilePath(LPDWORD lpdwFilePathLength)
{
	const LPCTSTR lpcsExt = ".dll";
	int iFilenameLength = lstrlen(EVENT_PROVIDER_NAME) + lstrlen(lpcsExt);
	const LPTSTR lpsFileName = new TCHAR[iFilenameLength + 1];
	lpsFileName[0] = '\0';
	strcat_s(lpsFileName, iFilenameLength + 1, EVENT_PROVIDER_NAME);
	strcat_s(lpsFileName, iFilenameLength + 1, lpcsExt);
	LPTSTR lpsFilePath = new TCHAR[MAX_PATH + 1];
	*lpdwFilePathLength = GetFullPathName(lpsFileName, MAX_PATH + 1, lpsFilePath, NULL);
	delete[] lpsFileName;
	if (*lpdwFilePathLength == 0)
	{
		delete[] lpsFilePath;
		lpsFilePath = NULL;
	}
	return lpsFilePath;
}

BOOL InstallEventProvider()
{
	const LPTSTR lpcsKeyPath = GetFullRegistryKeyPath();

	HKEY hkProviderKey;
	DWORD dwDisposition;
	BOOL bResult;
	if ((RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpcsKeyPath, 0, NULL, 0, KEY_SET_VALUE, NULL, &hkProviderKey, &dwDisposition) == ERROR_SUCCESS)
		&& (dwDisposition == REG_CREATED_NEW_KEY))
	{
		DWORD dwFilePathLength;
		const LPTSTR lpsFilePath = GetFullFilePath(&dwFilePathLength);
		if (dwFilePathLength != 0)
		{
			const DWORD dwCategoryCount = 1, dwTypesSupported = EVENTLOG_ERROR_TYPE | EVENTLOG_INFORMATION_TYPE | EVENTLOG_WARNING_TYPE;
			bResult = (RegSetValueEx(hkProviderKey, "CategoryCount", 0, REG_DWORD, (const BYTE *)&dwCategoryCount, sizeof(DWORD)) == ERROR_SUCCESS)
				&& (RegSetValueEx(hkProviderKey, "CategoryMessageFile", 0, REG_SZ, (const BYTE *)lpsFilePath, (dwFilePathLength + 1) * sizeof(TCHAR)) == ERROR_SUCCESS)
				&& (RegSetValueEx(hkProviderKey, "EventMessageFile", 0, REG_SZ, (const BYTE *)lpsFilePath, (dwFilePathLength + 1) * sizeof(TCHAR)) == ERROR_SUCCESS)
				&& (RegSetValueEx(hkProviderKey, "ParameterMessageFile", 0, REG_SZ, (const BYTE *)lpsFilePath, (dwFilePathLength + 1) * sizeof(TCHAR)) == ERROR_SUCCESS)
				&& (RegSetValueEx(hkProviderKey, "TypesSupported", 0, REG_DWORD, (const BYTE *)&dwTypesSupported, sizeof(DWORD)) == ERROR_SUCCESS);
			delete[] lpsFilePath;
		}
		else
		{
			bResult = FALSE;
		}
	}
	else
	{
		bResult = FALSE;
	}

	delete[] lpcsKeyPath;
	return bResult;
}

BOOL InstallService()
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	TCHAR lptsPath[MAX_PATH];

	if (!GetModuleFileName(NULL, lptsPath, MAX_PATH))
	{
		return FALSE;
	}

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (NULL == hSCManager)
	{
		return FALSE;
	}

	hService = CreateService(hSCManager, boost_file_storage::lpcsServiceName, boost_file_storage::lpcsServiceDescription, SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		lptsPath, NULL, NULL, NULL, ".\\LocalSystem", NULL);

	if (hService == NULL)
	{
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return TRUE;
}

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		if (strcmp(argv[1], lpcsInstallArgument) == 0)
		{
			if (InstallService())
			{
				printf("Succesfully installed service\n");
				if (InstallEventProvider())
				{
					printf("Succesfully installed event provider\n");
					return 0;
				}
				else
				{
					printf("Error installing event provider\n");
					return 4;
				}
				return 0;
			}
			else
			{
				printf("Error while installing service: %d\n", GetLastError());
				return 4;
			}
		}
		else
		{
			printf("Unknown argument: %s\n", argv[1]);
			return 3;
		}
	}
	else if (argc > 2)
	{
		printf("Too much arguments\n");
		return 3;
	}
	else
	{
		return boost_file_storage::StartStorageService() ? 0 : 1;
	}

	return 0;
}
#endif
