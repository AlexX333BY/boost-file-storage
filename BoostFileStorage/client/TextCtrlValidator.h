#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace boost_file_storage
{
	class TextCtrlValidator : public wxValidator
	{
	public:
		TextCtrlValidator(wxString *data);
		virtual bool TransferFromWindow();
		virtual bool TransferToWindow();
	protected:
		wxString *m_data;
	};
}
