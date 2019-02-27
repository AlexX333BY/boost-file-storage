#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace boost_file_storage
{
	class ConnectionEvent : public wxEvent
	{
	public:
		ConnectionEvent(bool isConnected, int winId = 0, wxEventType eventType = wxEVT_NULL);
		bool GetConnectedStatus() const;
		virtual wxEvent *Clone() const;
	protected:
		const bool m_isConnected;
	};

	wxDEFINE_EVENT(SOCKET_CONNECTED_EVENT, ConnectionEvent);
	wxDEFINE_EVENT(SOCKET_DISCONNECTED_EVENT, ConnectionEvent);
}
