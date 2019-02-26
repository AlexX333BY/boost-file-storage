#include "FileStorageApp.h"
#include "FileStorageFrame.h"
#include "AddressChooserDialog.h"

namespace boost_file_storage
{
	bool FileStorageApp::OnInit()
	{
		(new FileStorageFrame("Frame test"))->Show(true);
		return true;
	}
}
