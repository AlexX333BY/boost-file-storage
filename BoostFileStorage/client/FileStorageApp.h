#pragma once

#define WIN32_LEAN_AND_MEAN

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace boost_file_storage
{
	class FileStorageApp : public wxApp
	{
	public:
		virtual bool OnInit();
	};
}
