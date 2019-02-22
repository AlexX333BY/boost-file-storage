#pragma once

#include "../server/server.h"
#include "ServiceArguments.h"

namespace boost_file_storage
{
	const LPCTSTR lpcsServiceName = "BoostFileStorage";
	const LPCTSTR lpcsServiceDescription = "Boost file storage";

	BOOL StartStorageService();
}
