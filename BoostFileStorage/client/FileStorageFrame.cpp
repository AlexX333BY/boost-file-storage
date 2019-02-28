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
		m_socket(nullptr), m_socket_thread(nullptr)
	{
		wxStatusBar *statusBar = CreateStatusBar();
		m_statusBarSizer = new wxBoxSizer(wxHORIZONTAL);

		m_sendingFileGauge = new wxGauge(statusBar, wxID_ANY, 0, wxDefaultPosition, 
			wxSize(statusBar->GetSize().GetWidth() / 10, wxDefaultSize.GetHeight()), wxGA_HORIZONTAL);
		m_sendingFileGauge->SetValue(0);
		m_sendingFileName = new wxStaticText(statusBar, wxID_ANY, wxEmptyString);
		
		m_statusBarSizer->Add(m_sendingFileGauge, 1, wxEXPAND | wxALIGN_LEFT | wxALL, border);
		m_statusBarSizer->Add(m_sendingFileName, 4, wxEXPAND | wxALL, border);
		statusBar->SetSizer(m_statusBarSizer);
		m_statusBarSizer->Show(false);

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
		Bind(wxEVT_BUTTON, &FileStorageFrame::OnDisconnectQuery, this);

		Bind(SOCKET_CONNECTED_EVENT, &FileStorageFrame::OnSocketConnected, this);
		Bind(SOCKET_DISCONNECTED_EVENT, &FileStorageFrame::OnSocketDisconnected, this);
		Bind(SOCKET_CONNECTING_EVENT, &FileStorageFrame::OnSocketConnecting, this);
		Bind(SOCKET_DISCONNECTING_EVENT, &FileStorageFrame::OnSocketDisconnecting, this);

		Bind(FILE_PROCESS_FAILURE_EVENT, &FileStorageFrame::OnFileProcessFailure, this);
		Bind(FILE_PROCESS_INFO_EVENT, &FileStorageFrame::OnFileProcessInfo, this);
		Bind(FILE_PROCESS_SUCCESS_EVENT, &FileStorageFrame::OnFileProcessSuccess, this);

		Bind(GAUGE_UPDATE_EVENT, &FileStorageFrame::OnStatusBarUpdate, this);

		m_socket = new client_socket(BUFSIZ);
	}

	FileStorageFrame::~FileStorageFrame()
	{
		if (!m_socket->is_closed())
		{
			m_socket->close();
		}

		if (m_socket_thread != nullptr)
		{
			m_fileQueueConditionVariable.notify_one();
			if (m_socket_thread->joinable())
			{
				m_socket_thread->join();
			}
			delete m_socket_thread;
		}

		delete m_socket;
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
				
				m_fileQueueMutex.lock();
				bool wasEmpty = m_fileQueue.empty();
				for (size_t i = 0; i < paths.GetCount(); ++i)
				{
					m_fileQueue.push(std::experimental::filesystem::path(paths[i].ToStdString()));
					Log(&m_logGenerator.GenerateAddFileMessage(paths[i]));
				}
				if (wasEmpty)
				{
					m_fileQueueConditionVariable.notify_one();
				}
				m_fileQueueMutex.unlock();
				NotifyStatusBarUpdate(0, paths.GetCount());
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
				m_fileQueueMutex.lock();
				bool wasEmpty = m_fileQueue.empty();
				for (size_t i = 0; i < files.GetCount(); ++i)
				{
					m_fileQueue.push(std::experimental::filesystem::path(files[i].ToStdString()));
					Log(&m_logGenerator.GenerateAddFileMessage(files[i]));
				}
				if (wasEmpty)
				{
					m_fileQueueConditionVariable.notify_one();
				}
				m_fileQueueMutex.unlock();
				NotifyStatusBarUpdate(0, files.GetCount());
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
				m_socket_thread = new std::thread(&FileStorageFrame::SocketListeningRoutine, this, addressDlg.GetAddress());
			}
		}
		else
		{
			event.Skip();
		}
	}

	void FileStorageFrame::OnDisconnectQuery(wxCommandEvent &event)
	{
		if (event.GetId() == m_disconnectButtonId)
		{
			m_socket->close();
			m_fileQueueConditionVariable.notify_one();
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
		if (m_socket_thread != nullptr)
		{
			if (m_socket_thread->joinable())
			{
				m_socket_thread->join();
			}
			delete m_socket_thread;
			m_socket_thread = nullptr;
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
		m_statusBarSizer->Show(false);
		m_sendingFileGauge->SetValue(0);
		m_sendingFileGauge->SetRange(m_fileQueue.size());
		m_sendingFileName->SetLabel("");
		Log(&m_logGenerator.GenerateDisconnectingMessage());
	}

	void FileStorageFrame::OnFileProcessSuccess(FileProcessEvent& event)
	{
		wxString message;
		switch (event.GetStatus())
		{
		case SENT:
			message = m_logGenerator.GenerateFileSentMessage(event.GetFilename());
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
			m_statusBarSizer->Show(true);
			m_statusBarSizer->Layout();
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

	void FileStorageFrame::OnStatusBarUpdate(StatusBarUpdateEvent &event)
	{
		int newValue = m_sendingFileGauge->GetValue() + event.GetValueIncrement(),
			newRange = m_sendingFileGauge->GetRange() + event.GetRangeIncrement();

		if (newValue >= newRange)
		{
			m_sendingFileGauge->SetValue(0);
			m_sendingFileGauge->SetRange(0);
			m_sendingFileName->SetLabel("");
			m_statusBarSizer->Show(false);
		}
		else
		{
			m_sendingFileGauge->SetValue(newValue);
			m_sendingFileGauge->SetRange(newRange);
		}

		m_sendingFileName->SetLabel(event.GetFilename());
	}

	void FileStorageFrame::NotifySocketConnection(ConnectionStatus status)
	{
		wxEventTypeTag<ConnectionEvent> tag = wxEVT_NULL;
		switch (status)
		{
		case STATUS_CONNECTED:
			tag = SOCKET_CONNECTED_EVENT;
			break;
		case STATUS_DISCONNECTED:
			tag = SOCKET_DISCONNECTED_EVENT;
			break;
		case STATUS_CONNECTING:
			tag = SOCKET_CONNECTING_EVENT;
			break;
		case STATUS_DISCONNECTING:
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

	void FileStorageFrame::NotifyStatusBarUpdate(int valueUpdate, int rangeUpdate, const wxString &filename)
	{
		StatusBarUpdateEvent *event = new StatusBarUpdateEvent(valueUpdate, rangeUpdate, filename, this->GetId(), GAUGE_UPDATE_EVENT);
		event->SetEventObject(this);
		QueueEvent(event);
	}

	void FileStorageFrame::SocketListeningRoutine(wxIPV4address address)
	{
		NotifySocketConnection(STATUS_CONNECTING);
		if (!m_socket->open() && !m_socket->connect(address.IPAddress().ToStdString(), address.Service()))
		{
			NotifySocketConnection(STATUS_CONNECTED);
			std::unique_lock<std::mutex> lock(m_fileQueueMutex, std::defer_lock);
			std::experimental::filesystem::path processFilePath;
			socket_message *message = nullptr;
			boost::system::error_code error;
			
			while (m_socket->is_connected())
			{
				lock.lock();
				while (m_fileQueue.empty() && !m_socket->is_closed())
				{
					m_fileQueueConditionVariable.wait(lock);
				}
				if (!m_socket->is_closed())
				{
					processFilePath = m_fileQueue.front();
					m_fileQueue.pop();
					lock.unlock();
					NotifyFileProcessed(CONSUMED, processFilePath.string());
					NotifyStatusBarUpdate(0, 0, processFilePath.filename().string());

					error.clear();
					message = QueryFileName(processFilePath, error);
					if (!error)
					{
						SendMessageResultNotification(message, &processFilePath.string());
						if (!message->is_error_message())
						{
							delete message;
							error.clear();
							message = SendFile(processFilePath, error);
							if (!error)
							{
								if (message != nullptr)
								{
									SendMessageResultNotification(message, &processFilePath.string());
									delete message;
								}
								else
								{
									NotifyFileProcessed(ILLEGAL_ACCESS, processFilePath.string());
								}
							}
							else
							{
								NotifySocketConnection(STATUS_DISCONNECTING);
								m_socket->close();
							}
						}
					}
					else
					{
						NotifySocketConnection(STATUS_DISCONNECTING);
						m_socket->close();
					}
					NotifyStatusBarUpdate(1, 0);
				}
				else
				{
					lock.unlock();
				}
			}
		}
		NotifySocketConnection(STATUS_DISCONNECTED);
	}

	socket_message *FileStorageFrame::QueryFileName(std::experimental::filesystem::path &filePath, boost::system::error_code &error)
	{
		std::experimental::filesystem::path processFileName = filePath.filename();
		socket_message message(UPLOAD_FILE_QUERY, processFileName.string().length(), processFileName.string().c_str());
		m_socket->send_message(&message, error);
		if (!error)
		{
			return m_socket->get_message(error);
		}
		else
		{
			return nullptr;
		}
	}

	socket_message *FileStorageFrame::SendFile(std::experimental::filesystem::path &filePath, boost::system::error_code &error)
	{
		boost::iostreams::mapped_file file;
		file.open(filePath.string(), boost::iostreams::mapped_file::mapmode::readonly);
		size_t size = file.size();
		if (file.is_open())
		{
			socket_message message(FILE, file.size(), file.const_data());
			m_socket->send_message(&message, error);
			if (!error)
			{
				return m_socket->get_message(error);
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

	void FileStorageFrame::SendMessageResultNotification(socket_message *message, std::string *filename)
	{
		switch (message->get_message_type())
		{
		case ERROR_TOO_BIG:
			if (filename != nullptr)
			{
				NotifyFileProcessed(TOO_BIG, *filename);
			}
			break;
		case ERROR_NO_SPACE:
			if (filename != nullptr)
			{
				NotifyFileProcessed(NO_SPACE, *filename);
			}
			break;
		case WARNING_NAME_EXISTS:
			if (filename != nullptr)
			{
				NotifyFileProcessed(SERVER_CHANGED_NAME, wxString::Format("%s\n%s", *filename,
					wxString((char *)message->get_buffer(), message->get_buffer_length())));
			}
			break;
		}
	}
}
