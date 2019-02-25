#include "FileStorageApp.h"
#include "FileStorageFrame.h"

namespace boost_file_storage
{
	bool FileStorageApp::OnInit()
	{
		FileStorageFrame *frame = new FileStorageFrame("Frame test");
		return frame->Show(true);
	}
}
