#include "socket.h"
#include <algorithm>
#include <exception>

namespace boost_file_storage
{
	socket::socket() : m_buffer_size(0), m_tcp_socket(nullptr)
	{ }

	boost::system::error_code socket::get_data(void *buffer, size_t buffer_size, size_t data_size)
	{
		boost::system::error_code error;
		if (buffer == nullptr)
		{
			return error;
		}
		boost::asio::read(*m_tcp_socket, boost::asio::buffer(buffer, buffer_size), boost::asio::transfer_exactly(data_size), error);
		return error;
	}

	boost::system::error_code socket::send_data(void *buffer, size_t buffer_size, size_t data_size)
	{
		boost::system::error_code error;
		if (buffer == nullptr)
		{
			return error;
		}
		boost::asio::write(*m_tcp_socket, boost::asio::buffer(buffer, buffer_size), boost::asio::transfer_exactly(data_size), error);
		return error;
	}

	socket_message *socket::get_message(boost::system::error_code &error)
	{
		message_type type;
		error = get_data(&type, sizeof(message_type), sizeof(message_type));
		if (error)
		{
			return nullptr;
		}

		size_t data_size;
		error = get_data(&data_size, sizeof(size_t), sizeof(size_t));
		if (error)
		{
			return nullptr;
		}

		void *data_buffer = nullptr;
		if ((data_size != 0) && (data_size <= m_buffer_size))
		{
			data_buffer = new unsigned char[data_size];
			error = get_data(data_buffer, data_size, data_size);
			if (error)
			{
				delete[] data_buffer;
				return nullptr;
			}
		}

		socket_message *message = new socket_message(type, data_size, data_buffer);
		if (data_buffer != nullptr)
		{
			delete[] data_buffer;
		}
		return message;
	}

	void socket::send_message(socket_message *message, boost::system::error_code &error)
	{
		message_type type = message->get_message_type();
		void *data = message->get_buffer();
		size_t data_size = message->get_buffer_length();

		error = send_data(&type, sizeof(message_type), sizeof(message_type));
		if (error)
		{
			return;
		}
		error = send_data(&data_size, sizeof(size_t), sizeof(size_t));
		if (error)
		{
			return;
		}
		if ((data_size != 0) && (data != nullptr))
		{
			error = send_data(data, data_size, data_size);
		}
	}

	boost::system::error_code socket::skip(size_t bytes_to_skip)
	{
		boost::system::error_code error;
		size_t cur_bytes_to_read, buffer_size = std::min(bytes_to_skip, m_buffer_size);
		void *buffer = new unsigned char[buffer_size];
		while (bytes_to_skip > 0)
		{
			cur_bytes_to_read = std::min(bytes_to_skip, buffer_size);
			error = get_data(buffer, buffer_size, cur_bytes_to_read);
			if (error)
			{
				delete[] buffer;
				return error;
			}
			bytes_to_skip -= cur_bytes_to_read;
		}
		delete[] buffer;
		return error;
	}

	size_t socket::get_buffer_size()
	{
		return m_buffer_size;
	}
}
