#pragma once
#include "socket.h"

namespace boost_file_storage
{
	enum server_socket_state
	{
		OPENED,
		CONNECTED,
		CLOSED
	};

	class server_socket : public socket
	{
	public:
		server_socket(unsigned short port, size_t desired_buffer_size);
		~server_socket();
		boost::system::error_code accept();
		bool open();
		virtual bool is_running();
		bool is_connected();
		virtual boost::system::error_code stop();
	protected:
		server_socket_state m_state;
		boost::asio::ip::tcp::acceptor *m_acceptor;
		boost::asio::io_context *m_context;
		boost::system::error_code m_accept_error;
		void accept_handler(const boost::system::error_code& error);
	};
}
