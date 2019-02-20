#include "client_socket.h"

namespace boost_file_storage
{
	client_socket::client_socket() : m_is_initialized(false), m_context(new boost::asio::io_context())
	{
		m_tcp_socket = new boost::asio::ip::tcp::socket(*m_context);
	}

	client_socket::~client_socket()
	{
		delete m_context;
	}

	bool client_socket::is_initialized()
	{
		return m_is_initialized;
	}

	bool client_socket::initialize(size_t desired_buffer_size)
	{
		if (!is_initialized())
		{
			size_t buffer_size = std::max(std::max(sizeof(message_type), sizeof(size_t)), desired_buffer_size);
			m_buffer = malloc(buffer_size);
			if (m_buffer == nullptr)
			{
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

	boost::system::error_code client_socket::connect(std::string ip, unsigned short port)
	{
		boost::system::error_code error;
		boost::asio::ip::address nonstr_ip = boost::asio::ip::address::from_string(ip, error);
		if (error)
		{
			return error;
		}

		boost::asio::ip::tcp::endpoint endpoint(nonstr_ip, port);
		m_tcp_socket->connect(endpoint, error);
		if (error)
		{
			return error;
		}
	}
}
