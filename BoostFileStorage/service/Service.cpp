#include "service.h"
#include <strsafe.h>

namespace boost_file_storage
{
	SERVICE_STATUS_HANDLE g_hServiceStatusHandle;
	SERVICE_STATUS g_ssServiceStatus;
	HANDLE g_hStopEvent;
	server *g_sStorageServer;

	const DWORD dwEventCreationError = 1;
	const DWORD dwServerStartupError = 2;
	const DWORD dwServerInitializeError = 3;
	const DWORD dwDefaultWaitHint = 1000;

	VOID ReportServiceStatus(DWORD dwCurrentState, DWORD dwCheckpoint = 0, DWORD dwWaitHint = dwDefaultWaitHint,
		DWORD dwWin32ExitCode = NO_ERROR, DWORD dwServiceSpecificExitCode = 0)
	{
		g_ssServiceStatus.dwCurrentState = dwCurrentState;
		g_ssServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
		g_ssServiceStatus.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
		g_ssServiceStatus.dwWaitHint = dwWaitHint;
		g_ssServiceStatus.dwCheckPoint = dwCheckpoint;

		if (dwCurrentState == SERVICE_START_PENDING)
		{
			g_ssServiceStatus.dwControlsAccepted = 0;
		}
		else
		{
			g_ssServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
		}

		SetServiceStatus(g_hServiceStatusHandle, &g_ssServiceStatus);
	}

	VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl)
	{
		switch (dwCtrl)
		{
		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP:
			ReportServiceStatus(SERVICE_STOP_PENDING, 0, 5 * dwDefaultWaitHint);
			g_sStorageServer->stop();
			ReportServiceStatus(SERVICE_STOP_PENDING, 1);
			delete g_sStorageServer;
			ReportServiceStatus(SERVICE_STOP_PENDING, 2);
			SetEvent(g_hStopEvent);
			return;
		default:
			break;
		}
	}

	VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
	{
		g_hServiceStatusHandle = RegisterServiceCtrlHandler(lpcsServiceName, ServiceCtrlHandler);

		if (g_hServiceStatusHandle == 0)
		{
			return;
		}

		g_ssServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		ReportServiceStatus(SERVICE_START_PENDING, 0);

		ServiceArguments saArguments;
		if (saArguments.SetData(dwArgc, lpszArgv))
		{
			ReportServiceStatus(SERVICE_START_PENDING, 1);

			g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			ReportServiceStatus(SERVICE_START_PENDING, 2);

			if (g_hStopEvent == NULL)
			{
				ReportServiceStatus(SERVICE_STOPPED, 0, 0, ERROR_SERVICE_SPECIFIC_ERROR, dwEventCreationError);
				return;
			}

			g_sStorageServer = new server();
			ReportServiceStatus(SERVICE_START_PENDING, 3);

			if (!g_sStorageServer->initialize(saArguments.GetListenPort(), *saArguments.GetDownloadFolder(),
				saArguments.GetMaxFileSize(), saArguments.GetSimultaneousDownloadCount()))
			{
				ReportServiceStatus(SERVICE_STOPPED, 0, 0, ERROR_SERVICE_SPECIFIC_ERROR, dwServerInitializeError);
				return;
			}

			ReportServiceStatus(SERVICE_START_PENDING, 4);

			if (!g_sStorageServer->start())
			{
				delete g_sStorageServer;
				ReportServiceStatus(SERVICE_STOPPED, 0, 0, ERROR_SERVICE_SPECIFIC_ERROR, dwServerStartupError);
				return;
			}

			ReportServiceStatus(SERVICE_RUNNING);

			WaitForSingleObject(g_hStopEvent, INFINITE);
		}
		else
		{
			ReportServiceStatus(SERVICE_STOPPED, 0, 0, ERROR_SERVICE_SPECIFIC_ERROR, dwServerInitializeError);
			return;
		}

		ReportServiceStatus(SERVICE_STOPPED);
	}

	BOOL StartStorageService()
	{
		int iServiceNameLength = lstrlen(lpcsServiceName);
		TCHAR *szServiceName = (TCHAR *)calloc(iServiceNameLength + 1, sizeof(TCHAR));
		strcpy_s(szServiceName, iServiceNameLength + 1, lpcsServiceName);

		SERVICE_TABLE_ENTRY aServiceStartTable[] =
		{
			{ szServiceName, ServiceMain },
			{ NULL, NULL }
		};

		return StartServiceCtrlDispatcher(aServiceStartTable);
	}
}
