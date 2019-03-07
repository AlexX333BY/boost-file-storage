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
			const int hGap = 5, const int vGap = 5, const int border = 10);
		wxIPV4address GetAddress();
	protected:
		void OnSubmit(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		wxIPV4address m_address;
		wxTextCtrl *m_ipControl, *m_portControl;

		const wxWindowID m_submitButtonId = wxID_APPLY, m_cancelButtonId = wxID_CANCEL;
	};
}
