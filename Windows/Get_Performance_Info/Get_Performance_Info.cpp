// Get_Performance_Info.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <Psapi.h>
#include <wchar.h>

int wmain(void) {

	PERFORMANCE_INFORMATION perf;
	// https://docs.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-performance_information

	DWORD cb = sizeof(perf);

	bool res = GetPerformanceInfo(&perf, cb);
	// https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-getperformanceinfo

	if (!res) {
		wprintf(L"Failed to retrieve Performance Information %ld", GetLastError());
		return 1;
	}

	int width = 15;
	int conversion = 1024 * 1025;  // MB
	
	wprintf(L"Current committed pages: %ld\n", perf.CommitTotal);
	wprintf(L"Current max pages: %ld\n", perf.CommitLimit);
	wprintf(L"Peak committed pages: %ld\n", perf.CommitPeak);
	wprintf(L"Current max pages: %ld\n", perf.CommitLimit);
	wprintf(L"Peak committed pages: %ld\n", perf.CommitPeak);
	wprintf(L"Available physical memory: %ld\n", perf.PhysicalAvailable);
	wprintf(L"Total physical memory: %ld\n", perf.PhysicalTotal);
	wprintf(L"System cache pages: %ld\n", perf.SystemCache);
	wprintf(L"Sum of paged and nonpaged kernel pools: %ld\n", perf.KernelTotal);
	wprintf(L"Paged kernel pool: %ld\n", perf.KernelPaged);
	wprintf(L"Nonpaged kernel pool: %ld\n", perf.KernelNonpaged);
	wprintf(L"Page size: %ld\n", perf.PageSize);
	wprintf(L"Handle count: %ld\n", perf.HandleCount);
	wprintf(L"Process count: %ld\n", perf.ProcessCount);
	wprintf(L"Threaddle count: %ld\n", perf.ThreadCount);



	//wprintf(L"Total virtual memory:%*I64d MB\n", width, mem.ullTotalVirtual / conversion);


	return 0;
}
