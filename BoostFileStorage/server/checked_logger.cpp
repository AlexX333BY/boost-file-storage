#include "checked_logger.h"

namespace boost_file_storage
{
	void log_if_logger_exists(std::shared_ptr<logger> logger, const std::string &message, log_message_type type)
	{
		if (logger)
		{
			switch (type)
			{
			case INFO_TYPE:
				logger->log_info(message);
				break;
			case ERROR_TYPE:
				logger->log_error(message);
				break;
			case WARNING_TYPE:
				logger->log_warning(message);
				break;
			}
		}
	}
}
