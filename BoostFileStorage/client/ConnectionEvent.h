#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace boost_file_storage
{
	enum ConnectionStatus
	{
		DISCONNECTED,
		CONNECTING,
		CONNECTED,
		DISCONNECTING
	};

	class ConnectionEvent : public wxEvent
	{
	public:
		ConnectionEvent(ConnectionStatus connectionStatus, int winId = 0, wxEventType eventType = wxEVT_NULL);
		ConnectionStatus GetConnectedStatus() const;
		virtual wxEvent *Clone() const;
	protected:
		const ConnectionStatus m_connectionStatus;
	};

	wxDEFINE_EVENT(SOCKET_CONNECTED_EVENT, ConnectionEvent);
	wxDEFINE_EVENT(SOCKET_DISCONNECTED_EVENT, ConnectionEvent);
}
