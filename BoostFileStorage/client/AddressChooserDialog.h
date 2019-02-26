#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/socket.h>

namespace boost_file_storage
{
	class AddressChooserDialog : public wxDialog
	{
	public:
		AddressChooserDialog(wxWindow *parent, wxWindowID id, const wxString &title, 
			const int h_gap = 5, const int v_gap = 5, const int border = 10);
		wxIPV4address getAddress();
	protected:
		void OnSubmit(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		wxIPV4address address;
		wxTextCtrl *m_host_control, *m_port_control;

		const wxWindowID submitButtonId = wxID_APPLY, cancelButtonId = wxID_CANCEL;
	};
}
