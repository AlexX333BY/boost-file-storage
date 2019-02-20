#pragma once
#include "socket.h"
#include <string>

namespace boost_file_storage
{
	class client_socket : public socket
	{
	public:
		client_socket();
		bool initialize(std::string ip, unsigned short port, size_t desired_buffer_size);
		virtual bool is_initialized();
	protected:
		boost::asio::io_context m_context;
		bool m_is_initialized;
	};
}
