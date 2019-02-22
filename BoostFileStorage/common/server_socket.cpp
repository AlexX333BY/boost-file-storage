#include "server_socket.h"

namespace boost_file_storage
{
	server_socket::server_socket(unsigned short port) : m_is_initialized(false), m_is_running(false), m_context(new boost::asio::io_context())
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
		m_acceptor = new boost::asio::ip::tcp::acceptor(*m_context, endpoint);
		m_tcp_socket = new boost::asio::ip::tcp::socket(*m_context);
	}

	server_socket::~server_socket()
	{
		if (is_initialized())
		{
			if (is_running())
			{
				stop();
			}
			free(m_buffer);
		}

		delete m_acceptor;
		delete m_tcp_socket;
		delete m_context;
	}

	bool server_socket::is_initialized()
	{
		return m_is_initialized;
	}

	bool server_socket::is_running()
	{
		return m_is_running;
	}

	bool server_socket::initialize(size_t desired_buffer_size)
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

	boost::system::error_code server_socket::accept()
	{
		throw_if_not_initialized();
		boost::system::error_code error;
		m_acceptor->accept(*m_tcp_socket, error);
		if (!error)
		{
			m_is_running = true;
		}
		return error;
	}

	boost::system::error_code server_socket::stop()
	{
		throw_if_not_initialized();
		boost::system::error_code error;
		m_acceptor->close(error);
		m_tcp_socket->shutdown(m_tcp_socket->shutdown_both, error);
		if (!error)
		{
			m_is_running = false;
			m_tcp_socket->close(error);
		}
		return error;
	}
}
