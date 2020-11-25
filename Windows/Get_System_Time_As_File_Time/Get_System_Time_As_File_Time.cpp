// Get_System_Time_As_File_Time.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <wchar.h>

int wmain(void) {

	FILETIME ft;
	// https://docs.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-filetime
	GetSystemTimeAsFileTime(&ft);
	// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtimeasfiletime#return-value

	LARGE_INTEGER li;  // a union to help with combining low and high parts of time
	// https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-large_integer~r1
	// https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-ularge_integer~r1

	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	long long int hns = li.QuadPart;
	wprintf(L"%lli hundreds of nanoseconds have elapsed "
		"since Windows API epoch\n", hns);

	return 0;
}