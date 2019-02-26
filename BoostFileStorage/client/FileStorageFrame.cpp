#include "FileStorageFrame.h"
#include "AddressChooserDialog.h"
#include <wx/dir.h>

namespace boost_file_storage
{
	const wxString m_connectCaption = "Connect";
	const wxString m_disconnectcCaption = "Disconnect";

	FileStorageFrame::FileStorageFrame(const wxString& title, const int border)
		: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE), m_socket(nullptr)
	{
		wxStatusBar *statusBar = CreateStatusBar();
		wxBoxSizer *statusBarSizer = new wxBoxSizer(wxHORIZONTAL);

		m_sendingFileGauge = new wxGauge(statusBar, wxID_ANY, 0, wxDefaultPosition, wxSize(statusBar->GetSize().GetWidth() / 10, wxDefaultSize.GetHeight()), wxGA_HORIZONTAL);
		m_sendingFileName = new wxStaticText(statusBar, wxID_ANY, wxEmptyString);
		m_sendingFileGauge->Show(false);
		m_sendingFileName->Show(false);

		statusBarSizer->Add(m_sendingFileGauge, 1, wxEXPAND | wxALIGN_LEFT | wxALL, border);
		statusBarSizer->Add(m_sendingFileName, 4, wxEXPAND | wxALIGN_LEFT | wxALL, border);
		statusBar->SetSizer(statusBarSizer);
		statusBar->Layout();

		wxPanel *panel = new wxPanel(this);
		m_connectButton = new wxButton(panel, m_connectButtonId, m_connectCaption);
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
					m_filesQueue.push(paths[i]);
					Log(&m_logGenerator.GenerateAddFileMessage(paths[i]));
				}
			}
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
					m_filesQueue.push(files[i]);
					Log(&m_logGenerator.GenerateAddFileMessage(files[i]));
				}
			}
		}
	}
}
