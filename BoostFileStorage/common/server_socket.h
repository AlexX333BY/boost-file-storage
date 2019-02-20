#pragma once
#include "socket.h"

namespace boost_file_storage
{
	class server_socket : public socket
	{
	public:
		server_socket();
		bool initialize(unsigned short port, size_t desired_buffer_size);
		virtual bool is_initialized();
	protected:
		boost::asio::io_context m_context;
		bool m_is_initialized;
	};
}
