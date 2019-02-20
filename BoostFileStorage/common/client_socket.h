#pragma once
#include "socket.h"
#include <string>

namespace boost_file_storage
{
	class client_socket : public socket
	{
	public:
		client_socket();
		~client_socket();
		bool initialize(size_t desired_buffer_size);
		virtual bool is_initialized();
		boost::system::error_code connect(std::string ip, unsigned short port);
	protected:
		boost::asio::io_context *m_context;
		bool m_is_initialized;
	};
}
