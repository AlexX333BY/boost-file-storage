#pragma once

#define WIN32_LEAN_AND_MEAN

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <queue>
#include "../common/client_socket.h"
#include "LogMessagesGenerator.h"
#include "ConnectionEvent.h"

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
		client_socket *m_socket;
		std::queue<wxString> m_filesQueue;

		wxGauge *m_sendingFileGauge;
		wxButton *m_connectButton;
		wxListBox *m_log;
		wxStaticText *m_sendingFileName;
		
		void OnFileAdd(wxCommandEvent& event);
		void OnFolderAdd(wxCommandEvent& event);
		void OnConnectQuery(wxCommandEvent& event);

		void OnSocketConnected(ConnectionEvent& event);
		void OnSocketDisconnected(ConnectionEvent& event);
		void OnSocketConnecting(ConnectionEvent& event);
		void OnSocketDisconnecting(ConnectionEvent& event);

		const int m_addFileButtonId = wxID_ADD, m_addFolderButtonId = wxID_ADD + 1, 
			m_connectButtonId = wxID_NETWORK, m_disconnectButtonId = wxID_NETWORK + 1;
	};
}
