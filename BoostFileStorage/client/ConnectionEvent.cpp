#include "ConnectionEvent.h"

namespace boost_file_storage
{
	ConnectionEvent::ConnectionEvent(ConnectionStatus connectionStatus, int winId, wxEventType eventType)
		: wxEvent(winId, eventType), m_connectionStatus(connectionStatus)
	{ }

	ConnectionStatus ConnectionEvent::GetConnectedStatus() const
	{
		return m_connectionStatus;
	}

	wxEvent *ConnectionEvent::Clone() const
	{
		return new ConnectionEvent(*this);
	}
}
