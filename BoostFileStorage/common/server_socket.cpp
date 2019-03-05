#include "server_socket.h"

namespace boost_file_storage
{
	server_socket::server_socket(size_t desired_buffer_size, boost::asio::io_context *context, boost::asio::ip::tcp::acceptor *acceptor)
		: socket(), m_state(CLOSED), m_context(context), m_acceptor(acceptor)
	{
		m_tcp_socket = nullptr;
		m_buffer_size = std::max(std::max(sizeof(message_type), sizeof(size_t)), desired_buffer_size);
	}

	server_socket::~server_socket()
	{
		if (!is_closed())
		{
			close();
		}
	}

	boost::system::error_code server_socket::open()
	{
		if (is_closed())
		{
			boost::system::error_code error;
			m_tcp_socket = new boost::asio::ip::tcp::socket(*m_context);
			m_state = OPENED;
			return error;
		}
		else
		{
			return boost::system::errc::make_error_code(boost::system::errc::device_or_resource_busy);
		}
	}

	boost::system::error_code server_socket::accept()
	{
		if (is_opened())
		{
			boost::system::error_code error;
			m_acceptor->wait(boost::asio::ip::tcp::acceptor::wait_read, error);
			if (!error)
			{
				m_acceptor->accept(*m_tcp_socket, error);
				if (!error)
				{
					m_state = CONNECTED;
				}
			}

			return error;
		}
		else if (is_connected())
		{
			return boost::system::errc::make_error_code(boost::system::errc::already_connected);
		}
		else
		{
			return boost::system::errc::make_error_code(boost::system::errc::no_stream_resources);
		}
	}

	boost::system::error_code server_socket::close()
	{
		boost::system::error_code error;
		if (!is_closed())
		{
			m_tcp_socket->cancel(error);
			m_tcp_socket->shutdown(m_tcp_socket->shutdown_both, error);
			m_tcp_socket->close(error);
			delete m_tcp_socket;
			m_tcp_socket = nullptr;
			m_state = CLOSED;
		}
		return error;
	}

	socket_state server_socket::get_state()
	{
		return m_state;
	}
}
