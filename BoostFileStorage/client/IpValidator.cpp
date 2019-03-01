#include "IpValidator.h"
#include <climits>
#include <wx/tokenzr.h>

namespace boost_file_storage
{
	IpValidator::IpValidator(wxString *data) : TextCtrlValidator(data)
	{ }

	bool IpValidator::Validate(wxWindow *)
	{
		wxWindow *window = GetWindow();
		if (window->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			wxStringTokenizer tokenizer(((wxTextCtrl *)window)->GetValue(), '.');
			if (tokenizer.CountTokens() == 4)
			{
				unsigned long value;
				while (tokenizer.HasMoreTokens())
				{
					if (!tokenizer.GetNextToken().ToULong(&value) || (value > UCHAR_MAX))
					{
						return false;
					}
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	wxObject* IpValidator::Clone() const
	{
		return new IpValidator(*this);
	}
}
