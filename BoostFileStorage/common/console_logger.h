#pragma once

#include "logger.h"

namespace boost_file_storage
{
	class console_logger : public logger
	{
	public:
		console_logger(const std::string &prefix);
		virtual void log(const std::string &message);
	protected:
		const std::string m_prefix;
	};
}
