#pragma once
#include "socket_message.h"

namespace boost_file_storage
{
	class socket
	{
	public:
		virtual socket_message *get_message() = 0;
		virtual void send_message(socket_message *message) = 0;
		virtual void disconnect() = 0;
	};
}
