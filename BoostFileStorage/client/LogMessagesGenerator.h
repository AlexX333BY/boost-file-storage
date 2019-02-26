#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace boost_file_storage
{
	class LogMessagesGenerator
	{
	public:
		virtual wxString GenerateAddFileMessage(const wxString filePath) const;
	};
}
