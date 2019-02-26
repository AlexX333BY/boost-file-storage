#include "TextCtrlValidator.h"
#include <climits>
#include <wx/tokenzr.h>

namespace boost_file_storage
{
	TextCtrlValidator::TextCtrlValidator(wxString *data) : wxValidator(), m_data(data)
	{ }

	bool TextCtrlValidator::TransferToWindow()
	{
		wxWindow *window = GetWindow();
		if (window->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			if (m_data)
			{
				((wxTextCtrl *)window)->SetValue(*m_data);
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	bool TextCtrlValidator::TransferFromWindow()
	{
		wxWindow *window = GetWindow();
		if (window->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			if (m_data)
			{
				*m_data = ((wxTextCtrl *)window)->GetValue();
			}

			return true;
		}
		else
		{
			return false;
		}
	}
}
