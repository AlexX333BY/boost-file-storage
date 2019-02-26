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
		virtual bool is_running();
		virtual boost::system::error_code stop();
	protected:
		boost::asio::io_context *m_context;
		bool m_is_running;
	};
}
