#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "TextCtrlValidator.h"

namespace boost_file_storage
{
	class IpValidator : public TextCtrlValidator
	{
	public:
		IpValidator(wxString *data);
		virtual bool Validate(wxWindow *parent);
		virtual wxObject* Clone() const;
	};
}
