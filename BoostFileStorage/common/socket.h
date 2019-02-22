#pragma once
#include "socket_message.h"
#include <boost/asio.hpp>

namespace boost_file_storage
{
	class socket
	{
	public:
		socket();
		virtual ~socket();
		size_t get_buffer_size();
		boost::system::error_code stop();
		bool is_running();
		socket_message *get_message(boost::system::error_code error);
		void send_message(socket_message *message, boost::system::error_code error);
		boost::system::error_code get_data(void *buffer, size_t buffer_size, size_t data_size);
		boost::system::error_code send_data(void *buffer, size_t buffer_size, size_t data_size);
		boost::system::error_code skip(size_t bytes_to_skip);
		virtual bool is_initialized() = 0;
	protected:
		void throw_if_not_initialized();
		boost::asio::ip::tcp::socket *m_tcp_socket;
		void *m_buffer;
		size_t m_buffer_size;
		bool m_is_running;
		boost::asio::io_context *m_context;
	};
}
