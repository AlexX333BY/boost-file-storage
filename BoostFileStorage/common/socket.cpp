#include "socket.h"
#include <algorithm>
#include <exception>

namespace boost_file_storage
{
	socket::socket() : m_buffer(nullptr), m_buffer_size(0), m_tcp_socket(nullptr)
	{ }

	void socket::throw_if_not_initialized()
	{
		if (!is_initialized())
		{
			throw new std::exception();
		}
	}

	boost::system::error_code socket::get_data(void *buffer, size_t buffer_size, size_t data_size)
	{
		throw_if_not_initialized();
		boost::system::error_code error;
		boost::asio::read(m_tcp_socket, boost::asio::buffer(buffer, buffer_size), boost::asio::transfer_exactly(data_size), error);
		return error;
	}

	boost::system::error_code socket::send_data(void *buffer, size_t buffer_size, size_t data_size)
	{
		throw_if_not_initialized();
		boost::system::error_code error;
		boost::asio::write(m_tcp_socket, boost::asio::buffer(buffer, buffer_size), boost::asio::transfer_exactly(data_size), error);
		return error;
	}

	socket_message *socket::get_message()
	{
		throw_if_not_initialized();
		get_data(m_buffer, m_buffer_size, sizeof(message_type));
		message_type type;
		memcpy_s(&type, sizeof(message_type), m_buffer, sizeof(message_type));

		get_data(m_buffer, m_buffer_size, sizeof(size_t));
		size_t data_size;
		memcpy_s(&data_size, sizeof(size_t), m_buffer, sizeof(size_t));

		void *data_buffer = nullptr;
		if ((data_size != 0) && (data_size <= m_buffer_size))
		{
			data_buffer = malloc(data_size);
			get_data(m_buffer, m_buffer_size, data_size);
			memcpy_s(data_buffer, data_size, m_buffer, data_size);
		}

		return new socket_message(type, data_size, data_buffer);
	}

	void socket::send_message(socket_message *message)
	{
		throw_if_not_initialized();
		message_type type = message->get_message_type();
		void *data = message->get_buffer();
		size_t data_size = message->get_buffer_length();

		send_data(&type, sizeof(message_type), sizeof(message_type));
		send_data(&data_size, sizeof(size_t), sizeof(size_t));
		if ((data_size != 0) && (data != nullptr))
		{
			send_data(data, data_size, data_size);
		}
	}

	void socket::skip(size_t bytes_to_skip)
	{
		throw_if_not_initialized();
		size_t cur_bytes_to_read;
		while (bytes_to_skip > 0)
		{
			cur_bytes_to_read = std::min(bytes_to_skip, m_buffer_size);
			get_data(m_buffer, m_buffer_size, cur_bytes_to_read);
			bytes_to_skip -= cur_bytes_to_read;
		}
	}

	size_t socket::get_buffer_size()
	{
		throw_if_not_initialized();
		return m_buffer_size;
	}

	boost::system::error_code socket::stop()
	{
		throw_if_not_initialized();
		boost::system::error_code error;
		m_tcp_socket->shutdown(m_tcp_socket->shutdown_both, error);
		m_tcp_socket->close(error);
		return error;
	}
}
