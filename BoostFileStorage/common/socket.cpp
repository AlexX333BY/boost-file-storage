#include "socket.h"
#include <algorithm>
#include <exception>

namespace boost_file_storage
{
	socket::socket() : m_buffer(nullptr), m_buffer_size(0), m_tcp_socket(nullptr)
	{ }

	socket::~socket()
	{
		if (is_initialized())
		{
			stop();
		}
		if (m_buffer != nullptr)
		{
			free(m_buffer);
		}
		if (m_tcp_socket != nullptr)
		{
			delete m_tcp_socket;
		}
	}

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

	socket_message *socket::get_message(boost::system::error_code error)
	{
		throw_if_not_initialized();
		error = get_data(m_buffer, m_buffer_size, sizeof(message_type));
		if (error)
		{
			return nullptr;
		}
		message_type type;
		memcpy_s(&type, sizeof(message_type), m_buffer, sizeof(message_type));

		error = get_data(m_buffer, m_buffer_size, sizeof(size_t));
		if (error)
		{
			return nullptr;
		}
		size_t data_size;
		memcpy_s(&data_size, sizeof(size_t), m_buffer, sizeof(size_t));

		void *data_buffer = nullptr;
		if ((data_size != 0) && (data_size <= m_buffer_size))
		{
			error = get_data(m_buffer, m_buffer_size, data_size);
			if (error)
			{
				return nullptr;
			}
			data_buffer = malloc(data_size);
			memcpy_s(data_buffer, data_size, m_buffer, data_size);
		}

		return new socket_message(type, data_size, data_buffer);
	}

	void socket::send_message(socket_message *message, boost::system::error_code error)
	{
		throw_if_not_initialized();
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
		throw_if_not_initialized();
		boost::system::error_code error;
		size_t cur_bytes_to_read;
		while (bytes_to_skip > 0)
		{
			cur_bytes_to_read = std::min(bytes_to_skip, m_buffer_size);
			error = get_data(m_buffer, m_buffer_size, cur_bytes_to_read);
			if (error)
			{
				return error;
			}
			bytes_to_skip -= cur_bytes_to_read;
		}
		return error;
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
		if (error)
		{
			return error;
		}
		m_tcp_socket->close(error);
		return error;
	}
}
