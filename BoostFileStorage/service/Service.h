#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace boost_file_storage
{
	const LPCTSTR lpcsServiceName = "BoostFileStorage";
	const LPCTSTR lpcsServiceDescription = "Boost file storage";
	const LPCTSTR lpcsEventProviderName = "BoostFileStorageEventProvider";

	BOOL StartStorageService();
}
