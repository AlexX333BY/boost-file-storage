#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace boost_file_storage
{
	class PortValidator : public wxValidator
	{
	public:
		PortValidator(wxString *data);
		virtual bool Validate();
	protected:
		const wxString *m_data;
	};
}
