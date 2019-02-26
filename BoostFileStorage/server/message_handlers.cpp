#include "message_handlers.h"
#include <fstream>

namespace boost_file_storage
{
	boost::system::error_code server_disconnect_handler(socket_message *, server_socket *socket, std::experimental::filesystem::path *, std::experimental::filesystem::path *)
	{
		return socket->stop();
	}

	boost::system::error_code server_file_query_handler(socket_message *client_message, server_socket *socket, 
		std::experimental::filesystem::path *download_folder, std::experimental::filesystem::path *file_path)
	{
		std::experimental::filesystem::path client_file_path, result_path;
		socket_message *message;
		boost::system::error_code error;
		client_file_path = std::experimental::filesystem::path(std::string((char *)client_message->get_buffer(), client_message->get_buffer_length()));
		if (!std::experimental::filesystem::exists(*download_folder / client_file_path))
		{
			message = new socket_message(OK, 0, nullptr);
			result_path = client_file_path;
		}
		else
		{
			result_path = client_file_path;
			for (int i = 0; std::experimental::filesystem::exists(*download_folder / result_path); ++i)
			{
				result_path = client_file_path.stem() / std::to_string(i) / client_file_path.extension();
			}

			std::string string_filename = result_path.string();
			message = new socket_message(WARNING_NAME_EXISTS, string_filename.length() * sizeof(char), string_filename.c_str());
		}
		file_path->assign(result_path.string());
		socket->send_message(message, error);
		delete message;
		return error;
	}

	boost::system::error_code server_file_handler(socket_message *client_message, server_socket *socket,
		std::experimental::filesystem::path *download_folder, std::experimental::filesystem::path *file_path)
	{
		socket_message *message;
		boost::system::error_code error;
		if (client_message->get_buffer() == nullptr)
		{
			message = new socket_message(ERROR_TOO_BIG, 0, nullptr);
		}
		else
		{
			std::experimental::filesystem::path absolute_path = *download_folder / *file_path;
			if (std::experimental::filesystem::space(absolute_path).available >= client_message->get_buffer_length())
			{
				if (std::experimental::filesystem::create_directories(absolute_path.parent_path()))
				{
					std::ofstream file(absolute_path.c_str());
					file.write((char *)client_message->get_buffer(), client_message->get_buffer_length());
					file.close();
					message = new socket_message(OK, 0, nullptr);
				}
				else
				{
					message = new socket_message(ERROR_COMMON, 0, nullptr);
				}
			}
			else
			{
				message = new socket_message(ERROR_NO_SPACE, 0, nullptr);
			}
		}
		
		socket->send_message(message, error);
		delete message;
		return error;
	}

	std::map<message_type, server_message_handler> get_server_handlers()
	{
		std::map<message_type, server_message_handler> map;
		map[DISCONNECT] = server_disconnect_handler;
		map[UPLOAD_FILE_QUERY] = server_file_query_handler;
		map[FILE] = server_file_handler;
		return map;
	}
}
