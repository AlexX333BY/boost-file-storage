#include "server_socket.h"

namespace boost_file_storage
{
	server_socket::server_socket() : m_is_initialized(false)
	{ }

	bool server_socket::is_initialized()
	{
		return m_is_initialized;
	}

	bool server_socket::initialize(unsigned short port, size_t desired_buffer_size)
	{
		if (!is_initialized())
		{
			size_t buffer_size = std::max(std::max(sizeof(message_type), sizeof(size_t)), desired_buffer_size);
			m_buffer = malloc(buffer_size);
			if (m_buffer == nullptr)
			{
				return false;
			}

			boost::system::error_code error;
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
			m_tcp_socket = new boost::asio::ip::tcp::socket(m_context);
			m_tcp_socket->connect(endpoint, error);
			if (error)
			{
				free(m_buffer);
				m_buffer = nullptr;
				delete m_tcp_socket;
				m_tcp_socket = nullptr;
				return false;
			}

			m_buffer_size = buffer_size;
			m_is_initialized = true;
			return true;
		}
		else
		{
			return false;
		}
	}
}
