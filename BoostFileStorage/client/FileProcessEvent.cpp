#include "FileProcessEvent.h"

namespace boost_file_storage
{
	FileProcessEvent::FileProcessEvent(FileProcessStatus status, wxString filename, int winId = 0, wxEventType eventType = wxEVT_NULL) 
		: wxEvent(winId, eventType), m_filename(filename), m_status(status)
	{ }

	wxString FileProcessEvent::GetFilename()
	{
		return m_filename;
	}

	FileProcessStatus FileProcessEvent::GetProcessStatus()
	{
		return m_status;
	}

	wxEvent *FileProcessEvent::Clone() const
	{
		return new FileProcessEvent(*this);
	}
}
