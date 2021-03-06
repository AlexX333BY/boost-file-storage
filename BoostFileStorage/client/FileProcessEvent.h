#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace boost_file_storage
{
	enum FileProcessStatus
	{
		CONSUMED,
		FILE_NOT_FOUND,
		ILLEGAL_ACCESS,
		SENT,
		TOO_BIG,
		SERVER_CHANGED_NAME,
		SERVER_DISCONNECTED,
		NO_SPACE,
		COMMON_ERROR
	};

	class FileProcessEvent : public wxEvent
	{
	public:
		FileProcessEvent(FileProcessStatus status, const wxString& filename, int winId = 0, wxEventType eventType = wxEVT_NULL);
		wxString GetFilename();
		FileProcessStatus GetStatus();
		virtual wxEvent *Clone() const;
	protected:
		FileProcessStatus m_status;
		wxString m_filename;
	};

	wxDEFINE_EVENT(FILE_PROCESS_INFO_EVENT, FileProcessEvent);
	wxDEFINE_EVENT(FILE_PROCESS_SUCCESS_EVENT, FileProcessEvent);
	wxDEFINE_EVENT(FILE_PROCESS_FAILURE_EVENT, FileProcessEvent);
}
