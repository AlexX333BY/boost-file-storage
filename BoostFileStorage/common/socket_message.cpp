#include "socket_message.h"
#include <stdlib.h>

namespace boost_file_storage
{
	socket_message::socket_message(message_type message_type, size_t data_buffer_length = 0, void *data_bufffer = nullptr)
		: m_message_type(message_type), m_buffer_length(data_buffer_length), m_buffer(data_bufffer)
	{ }

	socket_message::~socket_message()
	{
		free(m_buffer);
	}

	message_type socket_message::get_message_type()
	{
		return m_message_type;
	}

	size_t socket_message::get_buffer_length()
	{
		return m_buffer_length;
	}

	void *socket_message::get_buffer()
	{
		return m_buffer;
	}
}
