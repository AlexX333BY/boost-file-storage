#include "PortValidator.h"
#include <climits>

namespace boost_file_storage
{
	PortValidator::PortValidator(wxString *data) : TextCtrlValidator(data)
	{ }

	bool PortValidator::Validate(wxWindow *)
	{
		wxWindow *window = GetWindow();
		if (window->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			unsigned long value;
			return ((wxTextCtrl *)window)->GetValue().ToULong(&value) && (value <= USHRT_MAX);
		}
		else
		{
			return false;
		}
	}
}
