#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/socket.h>

namespace boost_file_storage
{
	class LogMessagesGenerator
	{
	public:
		virtual wxString GenerateAddFileMessage(const wxString& filePath) const;
		virtual wxString GenerateConnectAttemptMessage(const wxIPV4address& address) const;

		virtual wxString GenerateConnectingMessage() const;
		virtual wxString GenerateDisconnectingMessage() const;
		virtual wxString GenerateConnectedMessage() const;
		virtual wxString GenerateDisconnectedMessage() const;

		virtual wxString GenerateFileSentMessage(const wxString& filename) const;
		virtual wxString GenerateFileConsumedMessage(const wxString& filename) const;
		virtual wxString GenerateFileIllegalAccessMessage(const wxString& filename) const;
		virtual wxString GenerateFileNotFoundMessage(const wxString& filename) const;
		virtual wxString GenerateFileTooBigMessage(const wxString& filename) const;
		virtual wxString GenerateFileNameChangedMessage(const wxString& filename) const;
		virtual wxString GenerateServerDisconnectedWhileSendingFileMessage(const wxString& filename) const;
	};
}
