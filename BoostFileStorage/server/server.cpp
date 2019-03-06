#include "server.h"
#include "../common/socket_message.h"
#include "message_handlers.h"
#include "checked_logger.h"

namespace boost_file_storage
{
	server::server(logger *logger) : m_state(UNINITIALIZED), m_should_run(false), m_thread_count(0), m_logger(logger), 
		m_context(nullptr), m_acceptor(nullptr)
	{ }

	server::~server()
	{
		if (is_running())
		{
			stop();
		}
		else if (is_initialized())
		{
			clear_sockets();
		}

		if (m_logger != nullptr)
		{
			delete m_logger;
		}
	}

	void server::socket_routine(server_socket *socket)
	{
		boost::system::error_code error;
		socket_message *client_message;
		std::experimental::filesystem::path save_file_path;
		std::map<message_type, server_message_handler> handlers = get_server_handlers();

		while (m_should_run.load())
		{
			while (m_should_run.load() && !socket->is_opened())
			{
				error = socket->open();
				log_if_logger_exists(m_logger, "Open: " + error.message(), error ? ERROR_TYPE : INFO_TYPE);
			}
			while (m_should_run.load() && !socket->is_connected())
			{
				error = socket->accept();
				log_if_logger_exists(m_logger, "Accept: " + error.message(), error ? ERROR_TYPE : INFO_TYPE);
			}
			while (m_should_run.load() && socket->is_connected())
			{
				client_message = socket->get_message(error);
				log_if_logger_exists(m_logger, "Accept client message: " + error.message(), error ? ERROR_TYPE : INFO_TYPE);
				if (!error)
				{
					error = handlers.at(client_message->get_message_type())(client_message, socket, &m_download_folder, &save_file_path, m_logger);
					log_if_logger_exists(m_logger, "Handle client message: " + error.message(), error ? ERROR_TYPE : INFO_TYPE);
					delete client_message;
				}
				else
				{
					socket->close();
					log_if_logger_exists(m_logger, "Socket closed", INFO_TYPE);
				}
			}
		}
	}

	void server::clear_sockets()
	{
		for (std::vector<server_socket *>::const_iterator it = m_sockets.cbegin(); it != m_sockets.cend(); ++it)
		{
			delete *it;
		}
		m_sockets.clear();
	}

	void server::clear_threads()
	{
		for (std::vector<std::thread *>::const_iterator it = m_threads.cbegin(); it != m_threads.cend(); ++it)
		{
			if ((*it)->joinable())
			{
				(*it)->detach();
			}
			delete *it;
		}
		m_threads.clear();
	}

	bool server::initialize(unsigned short listen_port, std::string download_folder, size_t max_file_size, unsigned char max_simultaneous_downloads)
	{
		if (!is_initialized())
		{
			std::experimental::filesystem::path path(download_folder);
			if (std::experimental::filesystem::exists(path) && std::experimental::filesystem::is_directory(path))
			{
				m_context = new boost::asio::io_context();
				m_acceptor = new boost::asio::ip::tcp::acceptor(*m_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), listen_port));
				
				for (int i = 0; i < max_simultaneous_downloads; ++i)
				{
					m_sockets.push_back(new server_socket(max_file_size, m_context, m_acceptor));
				}

				m_download_folder = path;
				m_thread_count = max_simultaneous_downloads;
				m_state = INITIALIZED;
				log_if_logger_exists(m_logger, "Server successfully initialized", INFO_TYPE);
				return true;
			}
			else
			{
				log_if_logger_exists(m_logger, "Server initialize error: download path doesn't exist", ERROR_TYPE);
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	bool server::is_initialized()
	{
		return m_state != UNINITIALIZED;
	}

	bool server::is_running()
	{
		return m_state == RUNNING;
	}

	bool server::start()
	{
		if (is_initialized() && !is_running())
		{
			m_should_run.store(true);
			for (std::vector<server_socket *>::const_iterator it = m_sockets.cbegin(); it != m_sockets.cend(); ++it)
			{
				m_threads.push_back(new std::thread(&server::socket_routine, this, *it));
			}

			m_state = RUNNING;
			log_if_logger_exists(m_logger, "Server startup successfull", INFO_TYPE);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool server::stop()
	{
		if (is_running())
		{
			m_should_run.store(false);

			m_acceptor->close();
			for (std::vector<server_socket *>::const_iterator it = m_sockets.cbegin(); it != m_sockets.cend(); ++it)
			{
				(*it)->close();
			}

			for (std::vector<std::thread *>::const_iterator it = m_threads.cbegin(); it != m_threads.cend(); ++it)
			{
				(*it)->join();
			}
			clear_threads();
			clear_sockets();

			m_state = UNINITIALIZED;
			delete m_acceptor;
			delete m_context;
			log_if_logger_exists(m_logger, "Server stopped", INFO_TYPE);
			return true;
		}
		else
		{
			return false;
		}
	}
}
