#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace boost_file_storage
{
	class StatusBarUpdateEvent : public wxEvent
	{
	public:
		StatusBarUpdateEvent(int valueIncrement, int rangeIncrement, wxString filename = "", int winId = 0, wxEventType eventType = wxEVT_NULL);
		int GetValueIncrement() const;
		int GetRangeIncrement() const;
		wxString GetFilename() const;
		virtual wxEvent *Clone() const;
	protected:
		const int m_valueIncrement, m_rangeIncrement;
		const wxString m_filename;
	};

	wxDEFINE_EVENT(GAUGE_UPDATE_EVENT, StatusBarUpdateEvent);
}
