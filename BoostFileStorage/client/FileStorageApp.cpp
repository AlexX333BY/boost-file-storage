#include "FileStorageApp.h"
#include "FileStorageFrame.h"
#include "AddressChooserDialog.h"

namespace boost_file_storage
{
	bool FileStorageApp::OnInit()
	{
		(new FileStorageFrame("Boost File Storage"))->Show(true);
		return true;
	}
}
