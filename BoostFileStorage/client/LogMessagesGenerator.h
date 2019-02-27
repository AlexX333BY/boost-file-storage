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
		virtual wxString GenerateAddFileMessage(const wxString filePath) const;
		virtual wxString GenerateConnectAttemptMessage(const wxIPV4address *address) const;
	};
}
