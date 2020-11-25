// Global_Memory_Status_Ex.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <wchar.h>

int wmain(void) {

	MEMORYSTATUSEX mem;
	// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/ns-sysinfoapi-memorystatusex

	mem.dwLength = sizeof(mem);

	bool res = GlobalMemoryStatusEx(&mem);
	// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex

	if (!res) {
		wprintf(L"Failed to retrieve MemoryStatusEx %ld", GetLastError());
		return 1;
	}

	int width = 15;
	int conversion = 1024 * 1025;  // MB
	wprintf(L"Memory in use: %*I32d\n", width, mem.dwMemoryLoad);
	wprintf(L"Available physical memory:%*I64d MB\n", width, mem.ullAvailPhys / conversion);
	wprintf(L"Total physical memory:%*I64d MB\n", width, mem.ullTotalPhys / conversion);
	wprintf(L"Available page file:%*I64d MB\n", width, mem.ullAvailPageFile / conversion);
	wprintf(L"Total page file:%*I64d MB\n", width, mem.ullTotalPageFile / conversion);
	wprintf(L"Avalable virtual memory:%*I64d MB\n", width, mem.ullAvailVirtual / conversion);
	wprintf(L"Total virtual memory:%*I64d MB\n", width, mem.ullTotalVirtual / conversion);
	

	return 0;
}