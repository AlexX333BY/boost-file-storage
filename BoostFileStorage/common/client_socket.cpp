#include "client_socket.h"

namespace boost_file_storage
{
	client_socket::client_socket(size_t desired_buffer_size) 
		: socket(), m_context(new boost::asio::io_context()), m_state(CLOSED)
	{ 
		m_buffer_size = std::max(std::max(sizeof(message_type), sizeof(size_t)), desired_buffer_size);
		m_tcp_socket = nullptr;
	}

	client_socket::~client_socket()
	{
		if (!is_closed())
		{
			close();
		}

		delete m_context;
	}

	boost::system::error_code client_socket::connect(std::string ip, unsigned short port)
	{
		if (is_opened())
		{
			boost::system::error_code error;
			boost::asio::ip::address nonstr_ip = boost::asio::ip::address::from_string(ip, error);
			if (!error)
			{
				boost::asio::ip::tcp::endpoint endpoint(nonstr_ip, port);
				m_tcp_socket->connect(endpoint, error);
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

	boost::system::error_code client_socket::close()
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

	boost::system::error_code client_socket::open()
	{
		if (is_closed())
		{
			m_tcp_socket = new boost::asio::ip::tcp::socket(*m_context);
			m_state = OPENED;
			return boost::system::error_code();
		}
		else
		{
			return boost::system::errc::make_error_code(boost::system::errc::device_or_resource_busy);
		}
	}

	socket_state client_socket::get_state()
	{
		return m_state;
	}
}
