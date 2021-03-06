#pragma once
#include "../common/socket_message.h"
#include "../common/server_socket.h"
#include "../common/logger.h"
#include <map>
#include <filesystem>

namespace boost_file_storage
{
	typedef boost::system::error_code(*server_message_handler)(socket_message &message, server_socket &socket,
		std::experimental::filesystem::path &download_folder, std::experimental::filesystem::path &file_path, 
		std::shared_ptr<logger> logger);

	std::map<message_type, server_message_handler> get_server_handlers();
}
