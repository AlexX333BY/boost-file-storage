#include "ConnectionEvent.h"

namespace boost_file_storage
{
	ConnectionEvent::ConnectionEvent(bool isConnected, int winId, wxEventType eventType) 
		: wxEvent(winId, eventType), m_isConnected(isConnected)
	{ }

	bool ConnectionEvent::GetConnectedStatus() const
	{
		return m_isConnected;
	}

	wxEvent *ConnectionEvent::Clone() const
	{
		return new ConnectionEvent(*this);
	}
}
