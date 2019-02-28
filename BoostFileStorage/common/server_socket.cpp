#include "server_socket.h"

namespace boost_file_storage
{
	server_socket::server_socket(unsigned short port, size_t desired_buffer_size)
		: socket(), m_state(CLOSED), m_context(new boost::asio::io_context())
	{
		m_tcp_socket = nullptr;
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
		m_acceptor = new boost::asio::ip::tcp::acceptor(*m_context, endpoint);
		m_buffer_size = std::max(std::max(sizeof(message_type), sizeof(size_t)), desired_buffer_size);
	}

	server_socket::~server_socket()
	{
		if (is_running())
		{
			stop();
		}

		boost::system::error_code error;
		m_acceptor->close(error);
		m_tcp_socket->shutdown(m_tcp_socket->shutdown_both, error);
		m_tcp_socket->close(error);

		delete m_acceptor;
		delete m_tcp_socket;
		delete m_context;
	}

	bool server_socket::is_running()
	{
		return m_state != CLOSED;
	}

	bool server_socket::is_connected()
	{
		return m_state == CONNECTED;
	}

	bool server_socket::open()
	{
		if (!is_running())
		{
			if (m_tcp_socket != nullptr)
			{
				delete m_tcp_socket;
			}
			m_tcp_socket = new boost::asio::ip::tcp::socket(*m_context);
			m_state = OPENED;
			return true;
		}
		else
		{
			return false;
		}
	}

	void server_socket::accept_handler(const boost::system::error_code& error)
	{
		m_accept_error = error;
	}

	boost::system::error_code server_socket::accept()
	{
		boost::system::error_code error;
		if (is_running() && !is_connected())
		{
			m_acceptor->async_accept(*m_tcp_socket, std::bind(&server_socket::accept_handler, this, std::placeholders::_1));
			m_context->run_one(error);
			if (!error && !m_accept_error)
			{
				m_state = CONNECTED;
			}
			return error ? error : m_accept_error;
		}
		else
		{
			return boost::system::errc::make_error_code(boost::system::errc::already_connected);
		}
	}

	boost::system::error_code server_socket::stop()
	{
		boost::system::error_code error;
		if (is_running())
		{
			m_acceptor->cancel(error);
			m_tcp_socket->cancel(error);
			m_state = CLOSED;
		}
		return error;
	}
}
