#include "PortValidator.h"
#include <climits>

namespace boost_file_storage
{
	PortValidator::PortValidator(wxString *data) : wxValidator(), m_data(data)
	{ }

	bool PortValidator::Validate()
	{
		unsigned long value;
		return m_data->ToULong(&value) && (value <= USHRT_MAX);
	}
}
