#include "console_logger.h"

namespace boost_file_storage
{
	void console_logger::log_info(const std::string &message)
	{
		log(message, "INFO");
	}

	void console_logger::log_error(const std::string &message)
	{
		log(message, "ERROR");
	}

	void console_logger::log_warning(const std::string &message)
	{
		log(message, "WARNING");
	}

	void console_logger::log(const std::string &message, const std::string &str_type)
	{
		printf_s("%s %s: %s\n", m_prefix.c_str(), str_type.c_str(), message.c_str());
	}

	console_logger::console_logger(const std::string &prefix) : m_prefix(prefix)
	{ }
}
