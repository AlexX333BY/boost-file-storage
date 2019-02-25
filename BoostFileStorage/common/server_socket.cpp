#include "server_socket.h"

namespace boost_file_storage
{
	server_socket::server_socket(unsigned short port, size_t desired_buffer_size) 
		: socket(), m_is_running(false), m_context(new boost::asio::io_context())
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
		m_acceptor = new boost::asio::ip::tcp::acceptor(*m_context, endpoint);
		m_tcp_socket = new boost::asio::ip::tcp::socket(*m_context);
		m_buffer_size = std::max(std::max(sizeof(message_type), sizeof(size_t)), desired_buffer_size);
	}

	server_socket::~server_socket()
	{
		if (is_running())
		{
			stop();
		}

		delete m_acceptor;
		delete m_tcp_socket;
		delete m_context;
	}

	bool server_socket::is_running()
	{
		return m_is_running;
	}

	boost::system::error_code server_socket::accept()
	{
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
