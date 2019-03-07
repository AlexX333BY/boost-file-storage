#pragma once
#include "socket.h"
#include <string>

namespace boost_file_storage
{
	class client_socket : public socket
	{
	public:
		client_socket(size_t desired_buffer_size);
		~client_socket();
		boost::system::error_code connect(std::string ip, unsigned short port);
		virtual boost::system::error_code open();
		virtual boost::system::error_code close();
		virtual socket_state get_state();
	protected:
		std::unique_ptr<boost::asio::io_context> m_context;
		socket_state m_state;
		std::mutex m_close_mutex;
	};
}
