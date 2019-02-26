#pragma once

#define WIN32_LEAN_AND_MEAN

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "../common/client_socket.h"

namespace boost_file_storage
{
	class FileStorageFrame : public wxFrame
	{
	public:
		FileStorageFrame(const wxString& title, const int border = 5);
	protected:
		client_socket *socket;
		wxArrayString files_queue;
		wxGauge *m_sendingFileGauge;
		wxButton *m_connectButton, *m_addFileButton;
		wxListBox *m_fileList;
		wxStaticText *m_sendingFileName;
	};
}
