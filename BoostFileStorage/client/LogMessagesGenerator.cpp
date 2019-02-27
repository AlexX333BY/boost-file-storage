#include "LogMessagesGenerator.h"
#include <wx/tokenzr.h>

namespace boost_file_storage
{
	wxString LogMessagesGenerator::GenerateAddFileMessage(const wxString& filePath) const
	{
		return wxString::Format("File %s added to queue", filePath);
	}

	wxString LogMessagesGenerator::GenerateConnectAttemptMessage(const wxIPV4address& address) const
	{
		return wxString::Format("Attempting to connect to %s:%d...", address.IPAddress(), address.Service());
	}

	wxString LogMessagesGenerator::GenerateConnectingMessage() const
	{
		return wxString("Connecting...");
	}

	wxString LogMessagesGenerator::GenerateDisconnectingMessage() const
	{
		return wxString("Disconnecting...");
	}

	wxString LogMessagesGenerator::GenerateConnectedMessage() const
	{
		return wxString("Connected!");
	}

	wxString LogMessagesGenerator::GenerateDisconnectedMessage() const
	{
		return wxString("Disconnected!");
	}

	wxString LogMessagesGenerator::GenerateFileSentMessage(const wxString& filename) const
	{
		return wxString::Format("File %s sent to server", filename);
	}

	wxString LogMessagesGenerator::GenerateFileConsumedMessage(const wxString& filename) const
	{
		return wxString::Format("File %s is processing", filename);
	}

	wxString LogMessagesGenerator::GenerateFileIllegalAccessMessage(const wxString& filename) const
	{
		return wxString::Format("Cannot read file %s");
	}

	wxString LogMessagesGenerator::GenerateFileNotFoundMessage(const wxString& filename) const
	{
		return wxString::Format("File %s not found", filename);
	}

	wxString LogMessagesGenerator::GenerateFileTooBigMessage(const wxString& filename) const
	{
		return wxString::Format("Size of %s is too big for server", filename);
	}

	wxString LogMessagesGenerator::GenerateFileNameChangedMessage(const wxString& filename) const
	{
		wxStringTokenizer tokenizer(filename, '\n');
		wxString message = wxString::Format("Server changed name of file %s", tokenizer.GetNextToken());
		if (tokenizer.HasMoreTokens())
		{
			message.Append(wxString::Format(" to %s", tokenizer.GetNextToken()));
		}
		return message;
	}

	wxString LogMessagesGenerator::GenerateServerDisconnectedWhileSendingFileMessage(const wxString& filename) const
	{
		return wxString::Format("Server disconnected while sending file %s", filename);
	}

	wxString LogMessagesGenerator::GenerateNoSpaceMessage(const wxString &filename) const
	{
		return wxString::Format("Server has no space to store %s", filename);
	}
}
