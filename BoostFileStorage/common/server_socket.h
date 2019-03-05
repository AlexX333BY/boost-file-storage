#pragma once
#include "socket.h"
#include "socket_state.h"

namespace boost_file_storage
{
	class server_socket : public socket
	{
	public:
		server_socket(unsigned short port, size_t desired_buffer_size);
		~server_socket();
		boost::system::error_code accept();
		virtual boost::system::error_code open();
		virtual boost::system::error_code close();
		virtual socket_state get_state();
	protected:
		socket_state m_state;
		boost::asio::io_context m_context;
		boost::asio::ip::tcp::acceptor m_acceptor;
	};
}
