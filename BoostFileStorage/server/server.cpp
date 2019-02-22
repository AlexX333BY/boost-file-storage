#include "server.h"
#include "../common/socket_message.h"
#include "message_handlers.h"

namespace boost_file_storage
{
	server::server() : m_state(UNINITIALIZED), m_should_run(false), m_thread_count(0)
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
	}

	void socket_thread(server_socket *socket, std::atomic_bool *should_run, std::experimental::filesystem::path *download_folder)
	{
		boost::system::error_code error;
		socket_message *client_message;
		std::experimental::filesystem::path save_file_path;
		bool is_client_connected = false;
		std::map<message_type, server_message_handler> handlers = get_server_handlers();

		while (should_run->load())
		{
			while (should_run->load() && !is_client_connected)
			{
				is_client_connected = socket->accept() ? true : false;
			}
			while (should_run->load() && is_client_connected)
			{
				client_message = socket->get_message(error);
				if (!error)
				{
					handlers.at(client_message->get_message_type())(client_message, socket, download_folder, &save_file_path);
					if (client_message->get_message_type() == DISCONNECT)
					{
						socket->stop();
						is_client_connected = false;
					}
					delete client_message;
				}
				else
				{
					socket->stop();
					is_client_connected = false;
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
				server_socket *socket;
				for (int i = 0; i < max_simultaneous_downloads; ++i)
				{
					socket = new server_socket(listen_port);
					if (!socket->initialize(max_file_size))
					{
						clear_sockets();
						return false;
					}
					m_sockets.push_back(socket);
				}

				m_download_folder = path;
				m_thread_count = max_simultaneous_downloads;
				m_state = INITIALIZED;
				return true;
			}
			else
			{
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
				m_threads.push_back(new std::thread(socket_thread, *it, &m_should_run, &m_download_folder));
			}

			m_state = RUNNING;
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

			for (std::vector<server_socket *>::const_iterator it = m_sockets.cbegin(); it != m_sockets.cend(); ++it)
			{
				if ((*it)->is_running())
				{
					(*it)->stop();
				}
			}
			clear_sockets();

			for (std::vector<std::thread *>::const_iterator it = m_threads.cbegin(); it != m_threads.cend(); ++it)
			{
				(*it)->join();
			}
			clear_threads();

			m_state = UNINITIALIZED;
			return true;
		}
		else
		{
			return false;
		}
	}
}
