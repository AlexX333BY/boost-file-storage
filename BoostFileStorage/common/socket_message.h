#pragma once

namespace boost_file_storage
{
	enum message_type
	{
		// common commands
		OK,
		DISCONNECT,
		FILE,
		ERROR_COMMON,

		// client commands
		UPLOAD_FILE_QUERY,

		//server commands
		ERROR_TOO_BIG,
		ERROR_NO_SPACE,
		WARNING_NAME_EXISTS
	};

	struct socket_message
	{
	public:
		socket_message(message_type message_type, size_t data_buffer_length = 0, void *data_bufffer = nullptr);
		~socket_message();
		message_type get_message_type();
		size_t get_buffer_length();
		void *get_buffer();
	protected:
		message_type m_message_type;
		size_t m_buffer_length;
		void *m_buffer;
	};
}
