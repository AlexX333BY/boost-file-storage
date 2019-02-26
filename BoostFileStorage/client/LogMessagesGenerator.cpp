#include "LogMessagesGenerator.h"

namespace boost_file_storage
{
	wxString LogMessagesGenerator::GenerateAddFileMessage(const wxString filePath) const
	{
		return "File " + filePath + " added to queue";
	}
}
