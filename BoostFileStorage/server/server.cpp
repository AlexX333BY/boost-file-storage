#include "server.h"
#include "../common/socket_message.h"
#include "message_handlers.h"

namespace boost_file_storage
{
	server::server(logger *logger) : m_state(UNINITIALIZED), m_should_run(false), m_thread_count(0), m_logger(logger)
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

		delete m_logger;
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
				log_if_logger_exists("Open: " + error.message());
			}
			while (m_should_run.load() && !socket->is_connected())
			{
				error = socket->accept();
				log_if_logger_exists("Accept: " + error.message());
			}
			while (m_should_run.load() && socket->is_connected())
			{
				client_message = socket->get_message(error);
				log_if_logger_exists("Accept message: " + error.message());
				if (!error)
				{
					error = handlers.at(client_message->get_message_type())(client_message, socket, &m_download_folder, &save_file_path, m_logger);
					log_if_logger_exists("Handling message: " + error.message());
					delete client_message;
				}
				else
				{
					socket->close();
					log_if_logger_exists("Socket closed");
				}
			}
		}
	}

	void server::log_if_logger_exists(const std::string &message)
	{
		if (m_logger != nullptr)
		{
			m_logger->log(message);
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
			log_if_logger_exists("Server is initializing");
			std::experimental::filesystem::path path(download_folder);
			if (std::experimental::filesystem::exists(path) && std::experimental::filesystem::is_directory(path))
			{
				server_socket *socket;
				for (int i = 0; i < max_simultaneous_downloads; ++i)
				{
					socket = new server_socket(listen_port, max_file_size);
					m_sockets.push_back(socket);
				}

				m_download_folder = path;
				m_thread_count = max_simultaneous_downloads;
				m_state = INITIALIZED;
				log_if_logger_exists("Successfully initialized");
				return true;
			}
			else
			{
				log_if_logger_exists("Initialize error: download path doesn't exist");
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
			log_if_logger_exists("Server is starting");
			m_should_run.store(true);
			for (std::vector<server_socket *>::const_iterator it = m_sockets.cbegin(); it != m_sockets.cend(); ++it)
			{
				m_threads.push_back(new std::thread(&server::socket_routine, this, *it));
			}

			m_state = RUNNING;
			log_if_logger_exists("Startup successfull");
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
			log_if_logger_exists("Server is stopping");
			m_should_run.store(false);

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
			log_if_logger_exists("Server stopped");
			return true;
		}
		else
		{
			return false;
		}
	}
}
