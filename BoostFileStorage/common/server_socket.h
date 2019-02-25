#pragma once
#include "socket.h"

namespace boost_file_storage
{
	class server_socket : public socket
	{
	public:
		server_socket(unsigned short port, size_t desired_buffer_size);
		~server_socket();
		boost::system::error_code accept();
		virtual bool is_running();
		virtual boost::system::error_code stop();
	protected:
		boost::asio::ip::tcp::acceptor *m_acceptor;
		boost::asio::io_context *m_context;
		bool m_is_running;
	};
}
