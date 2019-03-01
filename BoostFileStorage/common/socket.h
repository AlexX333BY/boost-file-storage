#pragma once
#include "socket_message.h"
#include "socket_state.h"
#include <boost/asio.hpp>

namespace boost_file_storage
{
	class socket
	{
	public:
		socket();
		size_t get_buffer_size();
		socket_message *get_message(boost::system::error_code &error);
		void send_message(socket_message *message, boost::system::error_code &error);
		boost::system::error_code get_data(void *buffer, size_t buffer_size, size_t data_size);
		boost::system::error_code send_data(void *buffer, size_t buffer_size, size_t data_size);
		boost::system::error_code skip(size_t bytes_to_skip);

		virtual boost::system::error_code open() = 0;
		virtual boost::system::error_code close() = 0;
		virtual bool is_opened();
		virtual bool is_closed();
		virtual bool is_connected();
		virtual socket_state get_state() = 0;
	protected:
		boost::asio::ip::tcp::socket *m_tcp_socket;
		size_t m_buffer_size;
	};
}
