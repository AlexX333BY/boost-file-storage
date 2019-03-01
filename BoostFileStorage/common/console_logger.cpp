#include "console_logger.h"

namespace boost_file_storage
{
	void console_logger::log(const std::string &message)
	{
		printf_s("%s: %s\n", m_prefix.c_str(), message.c_str());
	}

	console_logger::console_logger(const std::string &prefix) : m_prefix(prefix)
	{ }
}
