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
		void OnSubmit();
		void OnCancel();
		wxIPV4address address;
		wxTextCtrl *m_host_control, *m_port_control;
	};
}
