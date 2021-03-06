#include "service.h"
#include <strsafe.h>
#include "ServiceLogger.h"
#include "BoostFileStorageEventProvider.h"
#include "../server/server.h"
#include "ServiceArguments.h"

namespace boost_file_storage
{
	SERVICE_STATUS_HANDLE g_hServiceStatusHandle;
	SERVICE_STATUS g_ssServiceStatus;
	HANDLE g_hStopEvent;
	std::unique_ptr<server> g_sStorageServer;

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
		DWORD dwCheckPoint;
		switch (dwCtrl)
		{
		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP:
			dwCheckPoint = 0;
			if (g_sStorageServer)
			{
				ReportServiceStatus(SERVICE_STOP_PENDING, dwCheckPoint++, 5 * dwDefaultWaitHint);
				g_sStorageServer->stop();
				ReportServiceStatus(SERVICE_STOP_PENDING, dwCheckPoint++);
				g_sStorageServer.reset();
			}
			ReportServiceStatus(SERVICE_STOP_PENDING, dwCheckPoint);
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

			ServiceLogger *serviceLogger = new ServiceLogger();
			serviceLogger->Initialize(NULL, lpcsEventProviderName);
			g_sStorageServer.reset(new server(std::unique_ptr<logger>(static_cast<logger *>(serviceLogger))));
			ReportServiceStatus(SERVICE_START_PENDING, 3);

			if (!g_sStorageServer->initialize(saArguments.GetListenPort(), saArguments.GetDownloadFolder(),
				saArguments.GetMaxFileSize(), saArguments.GetSimultaneousDownloadCount()))
			{
				g_sStorageServer.reset();
				ReportServiceStatus(SERVICE_STOPPED, 0, 0, ERROR_SERVICE_SPECIFIC_ERROR, dwServerInitializeError);
				return;
			}

			ReportServiceStatus(SERVICE_START_PENDING, 4);

			if (!g_sStorageServer->start())
			{
				g_sStorageServer.reset();
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
		size_t sServiceNameLength = lstrlen(lpcsServiceName);
		LPTSTR szServiceName = new TCHAR[sServiceNameLength + 1];
		strcpy_s(szServiceName, sServiceNameLength + 1, lpcsServiceName);

		SERVICE_TABLE_ENTRY aServiceStartTable[] =
		{
			{ szServiceName, ServiceMain },
			{ NULL, NULL }
		};

		BOOL bResult = StartServiceCtrlDispatcher(aServiceStartTable);
		delete[] szServiceName;
		return bResult;
	}
}
