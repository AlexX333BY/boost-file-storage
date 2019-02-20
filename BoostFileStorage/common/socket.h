#pragma once
#include "socket_message.h"
#include <boost/asio.hpp>

namespace boost_file_storage
{
	class socket
	{
	public:
		socket();
		size_t get_buffer_size();
		virtual boost::system::error_code stop();
		socket_message *get_message();
		void send_message(socket_message *message);
		boost::system::error_code get_data(void *buffer, size_t buffer_size, size_t data_size);
		boost::system::error_code send_data(void *buffer, size_t buffer_size, size_t data_size);
		void skip(size_t bytes_to_skip);
		virtual bool is_initialized() = 0;
	protected:
		void throw_if_not_initialized();
		boost::asio::ip::tcp::socket *m_tcp_socket;
		void *m_buffer;
		size_t m_buffer_size;
	};
}
