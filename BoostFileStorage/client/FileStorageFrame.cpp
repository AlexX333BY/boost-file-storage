#include "FileStorageFrame.h"
#include "AddressChooserDialog.h"

namespace boost_file_storage
{
	const wxString m_connectCaption = "Connect";
	const wxString m_disconnectcCaption = "Disconnect";

	FileStorageFrame::FileStorageFrame(const wxString& title, const int border)
		: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE)
	{
		wxStatusBar *statusBar = CreateStatusBar();
		wxBoxSizer *statusBarSizer = new wxBoxSizer(wxHORIZONTAL);

		m_sendingFileGauge = new wxGauge(statusBar, wxID_ANY, 0, wxDefaultPosition, wxSize(statusBar->GetSize().GetWidth() / 10, wxDefaultSize.GetHeight()), wxGA_HORIZONTAL);
		m_sendingFileName = new wxStaticText(statusBar, wxID_ANY, "");
		m_sendingFileGauge->Show(false);
		m_sendingFileName->Show(false);

		statusBarSizer->Add(m_sendingFileGauge, 1, wxEXPAND | wxALIGN_LEFT | wxALL, border);
		statusBarSizer->Add(m_sendingFileName, 4, wxEXPAND | wxALIGN_LEFT | wxALL, border);
		statusBar->SetSizer(statusBarSizer);
		statusBar->Layout();

		wxPanel *panel = new wxPanel(this);
		m_connectButton = new wxButton(panel, wxID_ANY, m_connectCaption);
		const wxString addFileCaption = "Add file in queue";
		m_addFileButton = new wxButton(panel, wxID_ANY, addFileCaption);

		wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
		buttonsSizer->Add(m_connectButton, 0, wxALL | wxALIGN_LEFT, border);
		buttonsSizer->Add(m_addFileButton, 0, wxALL | wxALIGN_LEFT, border);

		m_fileList = new wxListBox(panel, wxID_ANY);

		wxBoxSizer *frameSizer = new wxBoxSizer(wxVERTICAL);
		frameSizer->Add(buttonsSizer, 0, wxALL | wxALIGN_LEFT, border);
		frameSizer->Add(m_fileList, 1, wxALL | wxEXPAND, border);

		panel->SetSizer(frameSizer);
	}
}
