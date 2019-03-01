#include "FileProcessEvent.h"

namespace boost_file_storage
{
	FileProcessEvent::FileProcessEvent(FileProcessStatus status, const wxString& filename, int winId, wxEventType eventType)
		: wxEvent(winId, eventType), m_filename(filename), m_status(status)
	{ }

	wxString FileProcessEvent::GetFilename()
	{
		return m_filename;
	}

	FileProcessStatus FileProcessEvent::GetStatus()
	{
		return m_status;
	}

	wxEvent *FileProcessEvent::Clone() const
	{
		return new FileProcessEvent(*this);
	}
}
