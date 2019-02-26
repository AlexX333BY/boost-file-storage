#include "AddressChooserDialog.h"
#include "IpValidator.h"
#include "PortValidator.h"

namespace boost_file_storage
{
	AddressChooserDialog::AddressChooserDialog(wxWindow *parent, wxWindowID id, const wxString &title, 
		const int h_gap, const int v_gap, const int border)
		: wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
	{
		const int row_count = 3, column_count = 2;
		const wxString host_hint = "Host:", port_hint = "Port:", submit_hint = "Submit", cancel_hint = "Cancel";
		
		wxPanel *panel = new wxPanel(this);
		m_host_control = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, IpValidator(nullptr));
		m_port_control = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, PortValidator(nullptr));

		wxFlexGridSizer *sizer = new wxFlexGridSizer(row_count, column_count, v_gap, h_gap);
		
		sizer->Add(new wxStaticText(panel, wxID_ANY, host_hint), 0, wxALIGN_CENTER);
		sizer->Add(m_host_control, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL);
		sizer->Add(new wxStaticText(panel, wxID_ANY, port_hint), 0, wxALIGN_CENTER);
		sizer->Add(m_port_control, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL);

		sizer->Add(new wxButton(panel, cancelButtonId, cancel_hint), 1, wxALIGN_CENTER);
		sizer->Add(new wxButton(panel, submitButtonId, submit_hint), 1, wxALIGN_CENTER);

		for (int i = 0; i < row_count; ++i)
		{
			sizer->AddGrowableRow(i, 1);
		}
		sizer->AddGrowableCol(1, 1);

		wxBoxSizer *boxSizer = new wxBoxSizer(wxHORIZONTAL);
		boxSizer->Add(sizer, 1, wxALL | wxEXPAND, border);

		panel->SetSizer(boxSizer);

		Bind(wxEVT_BUTTON, &AddressChooserDialog::OnSubmit, this);
		Bind(wxEVT_BUTTON, &AddressChooserDialog::OnCancel, this);
	}

	void AddressChooserDialog::OnSubmit(wxCommandEvent& event)
	{
		if (event.GetId() == submitButtonId)
		{
			if (Validate() && address.Hostname(m_host_control->GetValue()) && address.Service(m_port_control->GetValue()))
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
		if (event.GetId() == cancelButtonId)
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

	wxIPV4address AddressChooserDialog::getAddress()
	{
		return address;
	}
}
