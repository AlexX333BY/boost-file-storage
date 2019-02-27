#include "LogMessagesGenerator.h"

namespace boost_file_storage
{
	wxString LogMessagesGenerator::GenerateAddFileMessage(const wxString filePath) const
	{
		return "File " + filePath + " added to queue";
	}

	wxString LogMessagesGenerator::GenerateConnectAttemptMessage(const wxIPV4address *address) const
	{
		return wxString::Format("Connecting to %s:%d...", address->Hostname(), address->Service());
	}
}
