#include "StatusBarUpdateEvent.h"

namespace boost_file_storage
{
	StatusBarUpdateEvent::StatusBarUpdateEvent(int valueIncrement, int rangeIncrement, wxString filename, int winId, wxEventType eventType)
		: wxEvent(winId, eventType), m_valueIncrement(valueIncrement), m_rangeIncrement(rangeIncrement), m_filename(filename)
	{ }

	int StatusBarUpdateEvent::GetValueIncrement() const
	{
		return m_valueIncrement;
	}

	int StatusBarUpdateEvent::GetRangeIncrement() const
	{
		return m_rangeIncrement;
	}

	wxString StatusBarUpdateEvent::GetFilename() const
	{
		return m_filename;
	}

	wxEvent *StatusBarUpdateEvent::Clone() const
	{
		return new StatusBarUpdateEvent(*this);
	}
}
