#pragma once
#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include "../common/server_socket.h"

namespace boost_file_storage
{
	enum server_state
	{
		UNINITIALIZED,
		INITIALIZED,
		RUNNING
	};

	class server
	{
	public:
		server();
		~server();
		bool initialize(unsigned short listen_port, std::string download_folder, size_t max_file_size, unsigned char max_simultaneous_downloads);
		bool is_initialized();
		bool is_running();
		void start();
		void stop();
	protected:
		std::vector<server_socket *> m_sockets;
		std::vector<std::thread *> m_threads;
		std::filesystem::path m_download_folder;
		unsigned char m_thread_count;
		server_state m_state;
	};
}
