#pragma once
#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include <atomic>
#include <memory>
#include "../common/server_socket.h"
#include "../common/logger.h"

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
		server(std::unique_ptr<logger> logger);
		~server();
		bool initialize(unsigned short listen_port, std::string download_folder, size_t max_file_size, unsigned char max_simultaneous_downloads);
		bool is_initialized();
		bool is_running();
		bool start();
		bool stop();
	protected:
		std::vector<std::shared_ptr<server_socket>> m_sockets;
		std::vector<std::unique_ptr<std::thread>> m_threads;
		std::experimental::filesystem::path m_download_folder;
		unsigned char m_thread_count;
		server_state m_state;
		std::atomic_bool m_should_run;
		std::shared_ptr<logger> m_logger;
		void socket_routine(std::shared_ptr<server_socket> socket);
		std::shared_ptr<boost::asio::io_context> m_context;
		std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
	};
}
