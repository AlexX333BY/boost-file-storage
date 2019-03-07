#pragma once

#include "../common/logger.h"

namespace boost_file_storage
{
	enum log_message_type
	{
		INFO_TYPE,
		ERROR_TYPE,
		WARNING_TYPE
	};

	void log_if_logger_exists(std::shared_ptr<logger> logger, const std::string &message, log_message_type type);
}
