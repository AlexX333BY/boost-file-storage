#include "LogMessagesGenerator.h"

namespace boost_file_storage
{
	wxString LogMessagesGenerator::GenerateAddFileMessage(const wxString filePath) const
	{
		return wxString::Format("File %s added to queue", filePath);
	}

	wxString LogMessagesGenerator::GenerateConnectAttemptMessage(const wxIPV4address *address) const
	{
		return wxString::Format("Attempting to connect to %s:%d...", address->IPAddress(), address->Service());
	}

	wxString LogMessagesGenerator::GenerateConnectingMessage()
	{
		return wxString("Connecting...");
	}

	wxString LogMessagesGenerator::GenerateDisconnectingMessage()
	{
		return wxString("Disconnecting...");
	}

	wxString LogMessagesGenerator::GenerateConnectedMessage()
	{
		return wxString("Connected!");
	}

	wxString LogMessagesGenerator::GenerateDisconnectedMessage()
	{
		return wxString("Disconnected!");
	}
}
