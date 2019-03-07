#pragma once
#include "socket.h"

namespace boost_file_storage
{
	class server_socket : public socket
	{
	public:
		server_socket(size_t desired_buffer_size, std::shared_ptr<boost::asio::io_context> context, 
			std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor);
		~server_socket();
		boost::system::error_code accept();
		virtual boost::system::error_code open();
		virtual boost::system::error_code close();
		virtual socket_state get_state();
	protected:
		socket_state m_state;
		std::shared_ptr<boost::asio::io_context> m_context;
		std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
		std::mutex m_close_mutex;
	};
}
