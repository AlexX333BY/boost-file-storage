#include "client_socket.h"

namespace boost_file_storage
{
	client_socket::client_socket(size_t desired_buffer_size) 
		: socket(), m_context(new boost::asio::io_context()), m_is_running(false)
	{ 
		m_buffer_size = std::max(std::max(sizeof(message_type), sizeof(size_t)), desired_buffer_size);
		m_tcp_socket = new boost::asio::ip::tcp::socket(*m_context);
	}

	client_socket::~client_socket()
	{
		if (is_running())
		{
			stop();
		}

		if (m_tcp_socket != nullptr)
		{
			delete m_tcp_socket;
		}
		delete m_context;
	}

	bool client_socket::is_running()
	{
		return m_is_running;
	}

	boost::system::error_code client_socket::connect(std::string ip, unsigned short port)
	{
		boost::system::error_code error;
		if (!is_running())
		{
			boost::asio::ip::address nonstr_ip = boost::asio::ip::address::from_string(ip, error);
			if (!error)
			{
				boost::asio::ip::tcp::endpoint endpoint(nonstr_ip, port);
				m_tcp_socket->connect(endpoint, error);
				if (!error)
				{
					m_is_running = true;
				}
			}
		}
		return error;
	}

	boost::system::error_code client_socket::stop()
	{
		boost::system::error_code error;
		if (is_running())
		{
			m_tcp_socket->shutdown(m_tcp_socket->shutdown_both, error);
			if (!error)
			{
				m_tcp_socket->close(error);
			}
			delete m_tcp_socket;
			m_tcp_socket = new boost::asio::ip::tcp::socket(*m_context);
			m_is_running = false;
			return error;
		}
		return error;
	}
}
