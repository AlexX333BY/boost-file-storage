#pragma once
#include "../common/socket_message.h"
#include "../common/server_socket.h"
#include <map>
#include <filesystem>

namespace boost_file_storage
{
	typedef boost::system::error_code(*server_message_handler)(socket_message *message, server_socket *socket,
		std::filesystem::path *download_folder, std::filesystem::path *file_path);

	std::map<message_type, server_message_handler> get_server_handlers();
}
