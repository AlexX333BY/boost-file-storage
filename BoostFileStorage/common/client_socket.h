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
		boost::system::error_code connect(std::string ip, unsigned short port);
		virtual bool is_running();
		virtual bool is_initialized();
		virtual boost::system::error_code stop();
	protected:
		boost::asio::io_context *m_context;
		bool m_is_initialized;
		bool m_is_running;
	};
}
