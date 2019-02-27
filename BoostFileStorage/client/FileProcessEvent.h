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
		SERVER_CHANGED_NAME
	};

	class FileProcessEvent : public wxEvent
	{
	public:
		FileProcessEvent(FileProcessStatus status, wxString filename, int winId = 0, wxEventType eventType = wxEVT_NULL);
		wxString GetFilename();
		FileProcessStatus GetProcessStatus();
		virtual wxEvent *Clone() const;
	protected:
		FileProcessStatus m_status;
		wxString m_filename;
	};

	wxDEFINE_EVENT(FILE_PROCESS_INFO_EVENT, FileProcessEvent);
	wxDEFINE_EVENT(FILE_PROCESS_SUCCESS_EVENT, FileProcessEvent);
	wxDEFINE_EVENT(FILE_PROCESS_FAILURE_EVENT, FileProcessEvent);
}
