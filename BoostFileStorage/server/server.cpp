#include "server.h"

namespace boost_file_storage
{
	server::server() : m_state(UNINITIALIZED)
	{
		m_should_run.store(false);
	}

	server::~server()
	{
		for (std::vector<server_socket *>::const_iterator it = m_sockets.cbegin; it != m_sockets.cend; ++it)
		{
			(*it)->stop();
			delete *it;
		}
		for (std::vector<std::thread *>::const_iterator it = m_threads.cbegin; it != m_threads.cend; ++it)
		{
			delete *it;
		}
	}

	void socket_thread(server_socket *socket, std::atomic_bool *should_run)
	{
		while (should_run->load())
		{
			// logic
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
			delete *it;
		}
		m_threads.clear();
	}

	bool server::initialize(unsigned short listen_port, std::string download_folder, size_t max_file_size, unsigned char max_simultaneous_downloads)
	{
		if (!is_initialized())
		{
			std::filesystem::path path(download_folder);
			if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
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
				m_threads.push_back(new std::thread(socket_thread, *it, &m_should_run));
			}

			m_state = RUNNING;
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
			m_should_run = false;

			for (std::vector<server_socket *>::const_iterator it = m_sockets.cbegin(); it != m_sockets.cend(); ++it)
			{
				(*it)->stop();
			}
			clear_sockets();
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
