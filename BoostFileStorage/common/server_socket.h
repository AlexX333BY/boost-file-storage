#pragma once
#include "socket.h"

namespace boost_file_storage
{
	class server_socket : public socket
	{
	public:
		server_socket(unsigned short port);
		~server_socket();
		bool initialize(size_t desired_buffer_size);
		boost::system::error_code accept();
		virtual bool is_running();
		virtual bool is_initialized();
		virtual boost::system::error_code stop();
	protected:
		boost::asio::ip::tcp::acceptor *m_acceptor;
		boost::asio::io_context *m_context;
		bool m_is_initialized;
		bool m_is_running;
	};
}
