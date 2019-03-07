#pragma once

#define WIN32_LEAN_AND_MEAN

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <queue>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include "../common/client_socket.h"
#include "LogMessagesGenerator.h"
#include "ConnectionEvent.h"
#include "FileProcessEvent.h"
#include "StatusBarUpdateEvent.h"

namespace boost_file_storage
{
	class FileStorageFrame : public wxFrame
	{
	public:
		FileStorageFrame(const wxString& title, const int border = 5);
		~FileStorageFrame();
	protected:
		LogMessagesGenerator m_logGenerator;
		void Log(const wxString *messages, unsigned int count = 1);
		std::unique_ptr<client_socket> m_socket;
		std::queue<std::experimental::filesystem::path> m_fileQueue;
		std::mutex m_fileQueueMutex;
		std::condition_variable m_fileQueueConditionVariable;
		std::unique_ptr<std::thread> m_socket_thread;

		wxButton *m_connectButton;
		wxListBox *m_log;
		wxStaticText *m_sendingFileName;
		wxGauge *m_sendingFileGauge;
		wxBoxSizer *m_statusBarSizer;
		wxStatusBar *m_statusBar;
		
		void OnFileAdd(wxCommandEvent &event);
		void OnFolderAdd(wxCommandEvent &event);
		void OnConnectQuery(wxCommandEvent &event);
		void OnDisconnectQuery(wxCommandEvent &event);

		void OnFileProcessSuccess(FileProcessEvent &event);
		void OnFileProcessFailure(FileProcessEvent &event);
		void OnFileProcessInfo(FileProcessEvent &event);
		void OnSocketConnected(ConnectionEvent &event);
		void OnSocketDisconnected(ConnectionEvent &event);
		void OnSocketConnecting(ConnectionEvent &event);
		void OnSocketDisconnecting(ConnectionEvent &event);
		void OnStatusBarUpdate(StatusBarUpdateEvent &event);

		void NotifySocketConnection(ConnectionStatus status);
		void NotifyFileProcessed(FileProcessStatus status, const wxString &filename);
		void NotifyStatusBarUpdate(int valueUpdate, int rangeUpdate = 0, const wxString &filename = "");

		void SocketListeningRoutine(wxIPV4address address);
		std::shared_ptr<socket_message> QueryFileName(std::experimental::filesystem::path &filePath, boost::system::error_code &error);
		std::shared_ptr<socket_message> SendFile(std::experimental::filesystem::path &filePath, boost::system::error_code &error);
		void SendMessageResultNotification(socket_message &message, const std::string &filename);

		const int m_addFileButtonId = wxID_ADD, m_addFolderButtonId = wxID_ADD + 1, 
			m_connectButtonId = wxID_NETWORK, m_disconnectButtonId = wxID_NETWORK + 1;
	};
}
