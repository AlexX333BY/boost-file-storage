#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace boost_file_storage
{
	class IpValidator : public wxValidator
	{
	public:
		IpValidator(wxString *data);
		virtual bool Validate();
	protected:
		const wxString *m_data;
	};
}
