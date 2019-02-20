#include "server.h"

namespace boost_file_storage
{
	server::server() : m_is_initialized(false)
	{ }

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

	void clear_socket_vector(std::vector<server_socket *> *vector)
	{
		for (std::vector<server_socket *>::const_iterator it = vector->cbegin(); it != vector->cend(); ++it)
		{
			delete *it;
		}
	}

	bool server::initialize(unsigned short listen_port, std::string download_folder, size_t max_file_size, unsigned char max_simultaneous_downloads)
	{
		if (!is_initialized())
		{
			boost::filesystem::path path(download_folder);
			if (boost::filesystem::exists(path) && boost::filesystem::is_directory(path))
			{
				server_socket *socket;
				for (int i = 0; i < max_simultaneous_downloads; ++i)
				{
					socket = new server_socket(listen_port);
					if (!socket->initialize(max_file_size))
					{
						clear_socket_vector(&m_sockets);
						m_sockets.clear();
						return false;
					}
					m_sockets.push_back(socket);
				}

				m_download_folder = path;
				m_thread_count = max_simultaneous_downloads;
				m_is_initialized = true;
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
		return m_is_initialized;
	}
}
