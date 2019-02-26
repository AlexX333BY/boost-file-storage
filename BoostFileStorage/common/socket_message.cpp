#include "socket_message.h"
#include <stdlib.h>
#include <memory.h>

namespace boost_file_storage
{
	socket_message::socket_message(message_type message_type, size_t data_buffer_length, const void *data_buffer)
		: m_message_type(message_type), m_buffer_length(data_buffer_length)
	{
		if (data_buffer != nullptr)
		{
			m_buffer = new unsigned char[m_buffer_length];
			memcpy_s(m_buffer, m_buffer_length, data_buffer, m_buffer_length);
		}
		else
		{
			m_buffer = nullptr;
		}
	}

	socket_message::~socket_message()
	{
		if (m_buffer != nullptr)
		{
			delete[] m_buffer;
		}
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
