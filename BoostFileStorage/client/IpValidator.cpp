#include "IpValidator.h"
#include <climits>
#include <wx/tokenzr.h>

namespace boost_file_storage
{
	IpValidator::IpValidator(wxString *data) : wxValidator(), m_data(data)
	{ }

	bool IpValidator::Validate()
	{
		wxStringTokenizer tokenizer(*m_data, '.');
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
}
