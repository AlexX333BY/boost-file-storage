#include "LogMessagesGenerator.h"

namespace boost_file_storage
{
	wxString LogMessagesGenerator::GenerateAddFileMessage(const wxString filePath) const
	{
		return wxString::Format("File %s added to queue", filePath);
	}

	wxString LogMessagesGenerator::GenerateConnectAttemptMessage(const wxIPV4address *address) const
	{
		return wxString::Format("Connecting to %s:%d...", address->Hostname(), address->Service());
	}
}
