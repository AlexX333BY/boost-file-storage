#pragma once

#include "logger.h"

namespace boost_file_storage
{
	class console_logger : public logger
	{
	public:
		console_logger(const std::string &prefix);
		virtual void log_info(const std::string &message);
		virtual void log_error(const std::string &message);
		virtual void log_warning(const std::string &message);
	protected:
		void log(const std::string &message, const std::string &str_type);
		const std::string m_prefix;
	};
}
