#include "FileStorageFrame.h"
#include "AddressChooserDialog.h"
#include <wx/dir.h>
#include <stdio.h>
#include <fstream>
#include <boost/iostreams/device/mapped_file.hpp>

namespace boost_file_storage
{
	const wxString connectCaption = "Connect";
	const wxString disconnectcCaption = "Disconnect";

	FileStorageFrame::FileStorageFrame(const wxString& title, const int border)
		: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE), 
		m_socket(nullptr), m_fileQueueMutex(new std::mutex()), m_fileQueueConditionVariable(new std::condition_variable())
	{
		wxStatusBar *statusBar = CreateStatusBar();
		wxBoxSizer *statusBarSizer = new wxBoxSizer(wxHORIZONTAL);

		m_sendingFileGauge = new wxGauge(statusBar, wxID_ANY, 0, wxDefaultPosition, 
			wxSize(statusBar->GetSize().GetWidth() / 10, wxDefaultSize.GetHeight()), wxGA_HORIZONTAL);
		m_sendingFileName = new wxStaticText(statusBar, wxID_ANY, wxEmptyString);
		m_sendingFileGauge->Show(false);
		m_sendingFileName->Show(false);

		statusBarSizer->Add(m_sendingFileGauge, 1, wxEXPAND | wxALIGN_LEFT | wxALL, border);
		statusBarSizer->Add(m_sendingFileName, 4, wxEXPAND | wxALIGN_LEFT | wxALL, border);
		statusBar->SetSizer(statusBarSizer);
		statusBar->Layout();

		wxPanel *panel = new wxPanel(this);
		m_connectButton = new wxButton(panel, m_connectButtonId, connectCaption);
		const wxString addFileCaption = "Add file in queue", addFolderCaption = "Add folder in queue";

		wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
		buttonsSizer->Add(m_connectButton, 0, wxALL | wxALIGN_LEFT, border);
		buttonsSizer->Add(new wxButton(panel, m_addFileButtonId, addFileCaption), 0, wxALL | wxALIGN_LEFT, border);
		buttonsSizer->Add(new wxButton(panel, m_addFolderButtonId, addFolderCaption), 0, wxALL | wxALIGN_LEFT, border);

		m_log = new wxListBox(panel, wxID_ANY);

		wxBoxSizer *frameSizer = new wxBoxSizer(wxVERTICAL);
		frameSizer->Add(buttonsSizer, 0, wxALL | wxALIGN_LEFT, border);
		frameSizer->Add(m_log, 1, wxALL | wxEXPAND, border);

		panel->SetSizer(frameSizer);

		Bind(wxEVT_BUTTON, &FileStorageFrame::OnFileAdd, this);
		Bind(wxEVT_BUTTON, &FileStorageFrame::OnFolderAdd, this);
		Bind(wxEVT_BUTTON, &FileStorageFrame::OnConnectQuery, this);

		Bind(SOCKET_CONNECTED_EVENT, &FileStorageFrame::OnSocketConnected, this);
		Bind(SOCKET_DISCONNECTED_EVENT, &FileStorageFrame::OnSocketDisconnected, this);
		Bind(SOCKET_CONNECTING_EVENT, &FileStorageFrame::OnSocketConnecting, this);
		Bind(SOCKET_DISCONNECTING_EVENT, &FileStorageFrame::OnSocketDisconnecting, this);
	}

	FileStorageFrame::~FileStorageFrame()
	{
		if (m_socket != nullptr)
		{
			if (m_socket->is_running())
			{
				m_socket->stop();
			}
			delete m_socket;
		}
		delete m_fileQueueConditionVariable;
		delete m_fileQueueMutex;
	}

	void FileStorageFrame::Log(const wxString *messages, unsigned int count)
	{
		m_log->InsertItems(count, messages, m_log->GetCount());
	}

	void FileStorageFrame::OnFileAdd(wxCommandEvent& event)
	{
		if (event.GetId() == m_addFileButtonId)
		{
			wxFileDialog fileDialog(this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr,
				wxFD_DEFAULT_STYLE | wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
			if (fileDialog.ShowModal() == wxID_OK)
			{
				wxArrayString paths;
				fileDialog.GetPaths(paths);
				
				m_fileQueueMutex->lock();
				bool wasEmpty = m_fileQueue.empty();
				for (size_t i = 0; i < paths.GetCount(); ++i)
				{
					m_fileQueue.push(std::experimental::filesystem::path(paths[i].ToStdString()));
					Log(&m_logGenerator.GenerateAddFileMessage(paths[i]));
				}
				if (wasEmpty)
				{
					m_fileQueueConditionVariable->notify_one();
				}
				m_fileQueueMutex->unlock();
			}
		}
		else
		{
			event.Skip();
		}
	}

	void FileStorageFrame::OnFolderAdd(wxCommandEvent& event)
	{
		if (event.GetId() == m_addFolderButtonId)
		{
			wxDirDialog dirDialog(this, wxDirSelectorPromptStr, wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
			if (dirDialog.ShowModal() == wxID_OK)
			{
				wxArrayString files;
				wxDir::GetAllFiles(dirDialog.GetPath(), &files, wxEmptyString, wxDIR_FILES | wxDIR_DIRS | wxDIR_NO_FOLLOW);
				m_fileQueueMutex->lock();
				bool wasEmpty = m_fileQueue.empty();
				for (size_t i = 0; i < files.GetCount(); ++i)
				{
					m_fileQueue.push(std::experimental::filesystem::path(files[i].ToStdString()));
					Log(&m_logGenerator.GenerateAddFileMessage(files[i]));
				}
				if (wasEmpty)
				{
					m_fileQueueConditionVariable->notify_one();
				}
				m_fileQueueMutex->unlock();
			}
		}
		else
		{
			event.Skip();
		}
	}

	void FileStorageFrame::OnConnectQuery(wxCommandEvent& event)
	{
		if (event.GetId() == m_connectButtonId)
		{
			AddressChooserDialog addressDlg(this, wxID_ANY, "Choose connect address");
			if (addressDlg.ShowModal() == wxID_APPLY)
			{
				Log(&m_logGenerator.GenerateConnectAttemptMessage(addressDlg.GetAddress()));
				m_socket = new client_socket(BUFSIZ);
				/* start thread-socket listener */
			}
		}
		else
		{
			event.Skip();
		}
	}

	void FileStorageFrame::OnSocketConnected(ConnectionEvent& event)
	{
		m_connectButton->SetLabel(disconnectcCaption);
		m_connectButton->SetId(m_disconnectButtonId);
		m_connectButton->Enable(true);
		Log(&m_logGenerator.GenerateConnectedMessage());
	}

	void FileStorageFrame::OnSocketDisconnected(ConnectionEvent& event)
	{
		if (m_socket != nullptr)
		{
			delete m_socket;
			m_socket = nullptr;
		}

		m_connectButton->SetLabel(connectCaption);
		m_connectButton->SetId(m_connectButtonId);
		m_connectButton->Enable(true);
		Log(&m_logGenerator.GenerateDisconnectedMessage());
	}

	void FileStorageFrame::OnSocketConnecting(ConnectionEvent& event)
	{
		m_connectButton->Enable(false);
		Log(&m_logGenerator.GenerateConnectingMessage());
	}

	void FileStorageFrame::OnSocketDisconnecting(ConnectionEvent& event)
	{
		m_connectButton->Enable(false);
		Log(&m_logGenerator.GenerateDisconnectingMessage());
	}

	void FileStorageFrame::OnFileProcessSuccess(FileProcessEvent& event)
	{
		wxString message;
		switch (event.GetStatus())
		{
		case SENT:
			message = m_logGenerator.GenerateFileSentMessage(event.GetFilename);
			break;
		default:
			message = "Unknown success message";
			break;
		}
		Log(&message);
	}

	void FileStorageFrame::OnFileProcessFailure(FileProcessEvent& event)
	{
		wxString message;
		switch (event.GetStatus())
		{
		case FILE_NOT_FOUND:
			message = m_logGenerator.GenerateFileNotFoundMessage(event.GetFilename());
			break;
		case ILLEGAL_ACCESS:
			message = m_logGenerator.GenerateFileIllegalAccessMessage(event.GetFilename());
			break;
		case TOO_BIG:
			message = m_logGenerator.GenerateFileTooBigMessage(event.GetFilename());
			break;
		case SERVER_DISCONNECTED:
			message = m_logGenerator.GenerateServerDisconnectedWhileSendingFileMessage(event.GetFilename());
			break;
		case NO_SPACE:
			message = m_logGenerator.GenerateNoSpaceMessage(event.GetFilename());
			break;
		case COMMON_ERROR:
			message = m_logGenerator.GenerateCommonErrorMessage(event.GetFilename());
			break;
		default:
			message = "Unknown failure message";
			break;
		}
		Log(&message);
	}

	void FileStorageFrame::OnFileProcessInfo(FileProcessEvent& event)
	{
		wxString message;
		switch (event.GetStatus())
		{
		case CONSUMED:
			message = m_logGenerator.GenerateFileConsumedMessage(event.GetFilename());
			break;
		case SERVER_CHANGED_NAME:
			message = m_logGenerator.GenerateFileNameChangedMessage(event.GetFilename());
			break;
		default:
			message = "Unknown info message";
			break;
		}
		Log(&message);
	}

	void FileStorageFrame::NotifySocketConnection(ConnectionStatus status)
	{
		wxEventTypeTag<ConnectionEvent> tag = wxEVT_NULL;
		switch (status)
		{
		case CONNECTED:
			tag = SOCKET_CONNECTED_EVENT;
			break;
		case DISCONNECTED:
			tag = SOCKET_DISCONNECTED_EVENT;
			break;
		case CONNECTING:
			tag = SOCKET_CONNECTING_EVENT;
			break;
		case DISCONNECTING:
			tag = SOCKET_DISCONNECTING_EVENT;
			break;
		}

		ConnectionEvent *event = new ConnectionEvent(status, this->GetId(), tag);
		event->SetEventObject(this);
		QueueEvent(event);
	}

	void FileStorageFrame::NotifyFileProcessed(FileProcessStatus status, const wxString &filename)
	{
		wxEventTypeTag<FileProcessEvent> tag = wxEVT_NULL;
		switch (status)
		{
		case CONSUMED:
		case SERVER_CHANGED_NAME:
			tag = FILE_PROCESS_INFO_EVENT;
			break;
		case SENT:
			tag = FILE_PROCESS_SUCCESS_EVENT;
			break;
		case FILE_NOT_FOUND:
		case ILLEGAL_ACCESS:
		case TOO_BIG:
		case SERVER_DISCONNECTED:
		case NO_SPACE:
		case COMMON_ERROR:
			tag = FILE_PROCESS_FAILURE_EVENT;
			break;
		}

		FileProcessEvent *event = new FileProcessEvent(status, filename, this->GetId(), tag);
		event->SetEventObject(this);
		QueueEvent(event);
	}

	void SocketListeningThread(client_socket *socket, wxIPV4address address, std::queue<std::experimental::filesystem::path> &fileQueue,
		std::mutex &fileMutex, std::condition_variable *fileConditionVariable, FileStorageFrame *parent)
	{
		parent->NotifySocketConnection(CONNECTING);
		if (!socket->connect(address.IPAddress().ToStdString, address.Service()))
		{
			parent->NotifySocketConnection(CONNECTED);
			std::unique_lock<std::mutex> lock(fileMutex, std::defer_lock);
			std::experimental::filesystem::path processFilePath;
			socket_message *message = nullptr;
			boost::system::error_code error;
			
			while (socket->is_running())
			{
				lock.lock();
				while (fileQueue.empty() && socket->is_running())
				{
					fileConditionVariable->wait(lock);
				}
				if (socket->is_running())
				{
					processFilePath = fileQueue.front();
					fileQueue.pop();
					lock.unlock();
					parent->NotifyFileProcessed(CONSUMED, processFilePath.string());

					error.clear();
					message = QueryFileName(socket, processFilePath, error);
					if (!error)
					{
						SendMessageResultNotification(message, parent, &processFilePath.string());
						if (!message->is_error_message())
						{
							delete message;
							error.clear();
							message = SendFile(socket, processFilePath, error);
							if (!error)
							{
								if (message != nullptr)
								{
									SendMessageResultNotification(message, parent, &processFilePath.string());
									delete message;
								}
								else
								{
									parent->NotifyFileProcessed(ILLEGAL_ACCESS, processFilePath.string());
								}
							}
							else
							{
								parent->NotifySocketConnection(DISCONNECTING);
								socket->stop();
							}
						}
					}
					else
					{
						parent->NotifySocketConnection(DISCONNECTING);
						socket->stop();
					}
				}
				else
				{
					lock.unlock();
				}
			}
		}
		parent->NotifySocketConnection(DISCONNECTED);
	}

	socket_message *QueryFileName(client_socket *socket, std::experimental::filesystem::path &filePath, boost::system::error_code &error)
	{
		std::experimental::filesystem::path processFileName = filePath.filename();
		socket_message message(UPLOAD_FILE_QUERY, processFileName.string().length(), processFileName.string().c_str());
		socket->send_message(&message, error);
		if (!error)
		{
			return socket->get_message(error);
		}
		else
		{
			return nullptr;
		}
	}

	socket_message *SendFile(client_socket *socket, std::experimental::filesystem::path &filePath, boost::system::error_code &error)
	{
		boost::iostreams::mapped_file file;
		boost::iostreams::mapped_file_params params;
		params.flags = boost::iostreams::mapped_file::mapmode::readonly;
		params.hint = nullptr;
		params.length = 0;
		params.mode = 0;
		params.new_file_size = 0;
		params.offset = 0;
		params.path = filePath.string();
		file.open(params);
		if (file.is_open())
		{
			socket_message message(FILE, file.size(), file.data());
			socket->send_message(&message, error);
			if (!error)
			{
				return socket->get_message(error);
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			return nullptr;
		}
	}

	void SendMessageResultNotification(socket_message *message, FileStorageFrame *frame, std::string *filename = nullptr)
	{
		switch (message->get_message_type())
		{
		case ERROR_TOO_BIG:
			if (filename != nullptr)
			{
				frame->NotifyFileProcessed(TOO_BIG, *filename);
			}
			break;
		case ERROR_NO_SPACE:
			if (filename != nullptr)
			{
				frame->NotifyFileProcessed(NO_SPACE, *filename);
			}
			break;
		case WARNING_NAME_EXISTS:
			if (filename != nullptr)
			{
				frame->NotifyFileProcessed(SERVER_CHANGED_NAME, wxString::Format("%s\n%s", &filename,
					wxString((char *)message->get_buffer(), message->get_buffer_length())));
			}
			break;
		}
	}
}
