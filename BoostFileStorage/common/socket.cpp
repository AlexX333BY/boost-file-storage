#include "socket.h"
#include <algorithm>

namespace boost_file_storage
{
	void socket::read_to_buffer(size_t bytes_to_read)
	{
		boost::asio::read(m_tcp_socket, boost::asio::buffer(m_buffer, m_buffer_max_size), boost::asio::transfer_exactly(bytes_to_read));
	}

	void socket::write_to_socket(void *what_to_write, size_t size)
	{
		boost::asio::write(m_tcp_socket, boost::asio::buffer(what_to_write, m_buffer_max_size), boost::asio::transfer_exactly(size));
	}

	socket_message *socket::get_message()
	{
		read_to_buffer(sizeof(message_type));
		message_type type;
		memcpy_s(&type, sizeof(message_type), m_buffer, sizeof(message_type));

		read_to_buffer(sizeof(size_t));
		size_t data_size;
		memcpy_s(&data_size, sizeof(size_t), m_buffer, sizeof(size_t));

		void *data_buffer = nullptr;
		if ((data_size != 0) && (data_size <= m_buffer_max_size))
		{
			data_buffer = malloc(data_size);
			memcpy_s(data_buffer, data_size, m_buffer, data_size);
		}

		return new socket_message(type, data_size, data_buffer);
	}

	void socket::send_message(socket_message *message)
	{
		message_type type = message->get_message_type();
		void *data = message->get_buffer();
		size_t data_size = (data == nullptr) ? 0 : message->get_buffer_length();

		write_to_socket(&type, sizeof(message_type));
		write_to_socket(&data_size, sizeof(size_t));
		if ((data_size != 0) && (data != nullptr))
		{
			write_to_socket(data, data_size);
		}
	}

	void socket::skip(size_t bytes_to_skip)
	{
		size_t cur_bytes_to_read;
		while (bytes_to_skip > 0)
		{
			cur_bytes_to_read = std::min(bytes_to_skip, m_buffer_max_size);
			read_to_buffer(cur_bytes_to_read);
			bytes_to_skip -= cur_bytes_to_read;
		}
	}
}
