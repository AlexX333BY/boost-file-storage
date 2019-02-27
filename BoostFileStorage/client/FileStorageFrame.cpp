#include "FileStorageFrame.h"
#include "AddressChooserDialog.h"
#include <wx/dir.h>
#include <stdio.h>

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
				for (size_t i = 0; i < paths.GetCount(); ++i)
				{
					m_fileQueue.push(paths[i]);
					Log(&m_logGenerator.GenerateAddFileMessage(paths[i]));
				}
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
				for (size_t i = 0; i < files.GetCount(); ++i)
				{
					m_fileQueue.push(files[i]);
					Log(&m_logGenerator.GenerateAddFileMessage(files[i]));
				}
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
			tag = FILE_PROCESS_FAILURE_EVENT;
			break;
		}

		FileProcessEvent *event = new FileProcessEvent(status, filename, this->GetId(), tag);
		event->SetEventObject(this);
		QueueEvent(event);
	}

	void SocketListeningThread(client_socket *socket, std::queue<wxString> *fileQueue,
		std::mutex *fileMutex, std::condition_variable *fileConditionVariable, FileStorageFrame *parent);
}
