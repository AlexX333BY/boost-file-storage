#pragma once
#include "socket_message.h"
#include <boost/asio.hpp>
#include <streambuf>

namespace boost_file_storage
{
	class socket
	{
	public:
		socket_message *get_message();
		void send_message(socket_message *message);
		void skip(size_t bytes_to_skip);
		size_t get_buffer_size();
		virtual void disconnect() = 0;
		virtual void shutdown() = 0;
	protected:
		void read_to_buffer(size_t bytes_to_read);
		void write_to_socket(void *what_to_write, size_t size);
		boost::asio::ip::tcp *m_tcp_socket;
		void *m_buffer;
		size_t m_buffer_max_size;
	};
}
