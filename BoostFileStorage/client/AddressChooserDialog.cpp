#include "AddressChooserDialog.h"
#include "IpValidator.h"
#include "PortValidator.h"

namespace boost_file_storage
{
	AddressChooserDialog::AddressChooserDialog(wxWindow *parent, wxWindowID id, const wxString &title, 
		const int hGap, const int vGap, const int border)
		: wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
	{
		const int rowCount = 3, columnCount = 2;
		const wxString hostHint = "Host:", portHint = "Port:", submitHint = "Submit", cancelHint = "Cancel";
		
		wxPanel *panel = new wxPanel(this);
		m_ipControl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, IpValidator(nullptr));
		m_portControl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, PortValidator(nullptr));

		wxFlexGridSizer *sizer = new wxFlexGridSizer(rowCount, columnCount, vGap, hGap);
		
		sizer->Add(new wxStaticText(panel, wxID_ANY, hostHint), 0, wxALIGN_CENTER);
		sizer->Add(m_ipControl, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL);
		sizer->Add(new wxStaticText(panel, wxID_ANY, portHint), 0, wxALIGN_CENTER);
		sizer->Add(m_portControl, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL);

		sizer->Add(new wxButton(panel, m_cancelButtonId, cancelHint), 1, wxALIGN_CENTER);
		sizer->Add(new wxButton(panel, m_submitButtonId, submitHint), 1, wxALIGN_CENTER);

		for (int i = 0; i < rowCount; ++i)
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
