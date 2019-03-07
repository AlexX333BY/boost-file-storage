#include "AddressChooserDialog.h"
#include "IpValidator.h"
#include "PortValidator.h"

namespace boost_file_storage
{
	AddressChooserDialog::AddressChooserDialog(wxWindow *parent, wxWindowID id, const wxString &title, 
		const int hGap, const int vGap, const int border)
		: wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
	{
		const int rowCount = 2, columnCount = 2;
		const wxString hostHint = "Host:", portHint = "Port:", submitHint = "Submit", cancelHint = "Cancel";
		
		wxPanel *panel = new wxPanel(this);
		m_ipControl = std::make_unique<wxTextCtrl>(wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, IpValidator(nullptr)));
		m_portControl = std::make_unique<wxTextCtrl>(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, PortValidator(nullptr));

		wxFlexGridSizer *inputSizer = new wxFlexGridSizer(rowCount, columnCount, vGap, hGap);
		
		inputSizer->Add(new wxStaticText(panel, wxID_ANY, hostHint), 0, wxALIGN_CENTER);
		inputSizer->Add(m_ipControl.get(), 1, wxEXPAND | wxALIGN_CENTER_VERTICAL);
		inputSizer->Add(new wxStaticText(panel, wxID_ANY, portHint), 0, wxALIGN_CENTER);
		inputSizer->Add(m_portControl.get(), 1, wxEXPAND | wxALIGN_CENTER_VERTICAL);

		wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
		buttonSizer->Add(new wxButton(panel, m_cancelButtonId, cancelHint), 1, wxALIGN_LEFT | wxLEFT | wxRIGHT, hGap);
		buttonSizer->Add(new wxButton(panel, m_submitButtonId, submitHint), 1, wxALIGN_LEFT | wxLEFT | wxRIGHT, hGap);

		for (int i = 0; i < rowCount; ++i)
		{
			inputSizer->AddGrowableRow(i, 1);
		}
		inputSizer->AddGrowableCol(1, 1);

		wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
		panelSizer->Add(inputSizer, 1, wxALL | wxEXPAND, border);
		panelSizer->Add(buttonSizer, 0, wxALL, border);

		panel->SetSizer(panelSizer);

		Bind(wxEVT_BUTTON, &AddressChooserDialog::OnSubmit, this);
		Bind(wxEVT_BUTTON, &AddressChooserDialog::OnCancel, this);
	}

	void AddressChooserDialog::OnSubmit(wxCommandEvent& event)
	{
		if (event.GetId() == m_submitButtonId)
		{
			if (Validate() && m_address.Hostname(m_ipControl->GetValue()) && m_address.Service(m_portControl->GetValue()))
			{
				if (IsModal())
					EndModal(wxID_APPLY);
				else
				{
					SetReturnCode(wxID_APPLY);
					this->Show(false);
				}
			}
		}
		else
		{
			event.Skip();
		}
	}

	void AddressChooserDialog::OnCancel(wxCommandEvent& event)
	{
		if (event.GetId() == m_cancelButtonId)
		{
			if (IsModal())
				EndModal(wxID_CANCEL);
			else
			{
				SetReturnCode(wxID_CANCEL);
				this->Show(false);
			}
		}
		else
		{
			event.Skip();
		}
	}

	wxIPV4address AddressChooserDialog::GetAddress()
	{
		return m_address;
	}
}
