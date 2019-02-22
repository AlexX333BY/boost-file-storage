#ifdef _DEBUG 
#include "../server/server.h"
#include <string>

using namespace boost_file_storage;

int main()
{
	const std::string dbg_download_dir = "C:\\Users\\user\\Downloads";
	const unsigned char dbg_threads = 16;
	const unsigned short dbg_port = 8080;
	const size_t dbg_max_file_size = 70000;

	server *s = new server();
	bool is_initialized = s->initialize(dbg_port, dbg_download_dir, dbg_max_file_size, dbg_threads);
	printf("Is server initialized? %s\n", is_initialized ? "TRUE" : "FALSE");
	if (is_initialized) {
		bool is_started = s->start();
		printf("Is server started? %s\n", is_started ? "TRUE" : "FALSE");
		if (is_started)
		{
			bool is_stopped = s->stop();
			printf("Is server stopped? %s\n", is_stopped ? "TRUE" : "FALSE");
		}
	}
	delete s;
	return 0;
}
#else
#include <stdio.h>
#include <Windows.h>
#include "service.h"

const LPCTSTR lpcInstallArgument = "-i";

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
		lptsPath, NULL, NULL, NULL, "NT AUTHORITY\\NetworkService", NULL);

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
		if (strcmp(argv[1], lpcInstallArgument) == 0)
		{
			if (InstallService())
			{
				printf("Succesfully installed\n");
			}
			else
			{
				printf("Error while installing service: %d\n", GetLastError());
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
		printf("Cannot process so much arguments\n");
		return 2;
	}
	else
	{
		return boost_file_storage::StartStorageService() ? 0 : 1;
	}

	return 0;
}
#endif
