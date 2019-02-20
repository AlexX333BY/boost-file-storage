#pragma once
#include <string>
#include <vector>
#include <thread>
#include <boost/filesystem.hpp>
#include "../common/server_socket.h"

namespace boost_file_storage
{
	class server
	{
	public:
		server();
		~server();
		bool initialize(unsigned short listen_port, std::string download_folder, size_t max_file_size, unsigned char max_simultaneous_downloads);
		bool is_initialized();
		void start();
		void stop();
	protected:
		std::vector<server_socket *> m_sockets;
		std::vector<std::thread *> m_threads;
		boost::filesystem::path m_download_folder;
		unsigned char m_thread_count;
		bool m_is_initialized;
	};
}
