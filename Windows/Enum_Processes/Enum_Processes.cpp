// Enum_Processes.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <string>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1


void PrintProcessNameAndID(DWORD processID) {
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	// Get a handle to the process.
	// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

	// Get the process name.
	if (NULL != hProcess) {
		HMODULE hMod;
		DWORD cbNeeded;
		// https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-enumprocessmodules
		// https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-getmodulebasenamea
		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
			GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
		}
	}

	// Print the process name and identifier.
	_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);

	//DWORD len = GetProcessImageFileNameA(HANDLE hProcess, LPSTR  lpImageFileName, DWORD  nSize );
	//TCHAR nameProc[MAX_PATH];
	//if (GetProcessImageFileName(hProcess, nameProc, sizeof(nameProc) / sizeof(*nameProc)) == 0) { return; }
	//printf("%ws\n", nameProc);


	// Release the handle to the process.
	CloseHandle(hProcess);
}

int main(void)
{
	// array of process identifiers, size of aProcesses, number of returned processes
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	// get processes
	// https://docs.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-enumprocesses?redirectedfrom=MSDN
	// cbNeeded contains the number of bytes in the returned process ID array (aProcesses)
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) { return 1; }

	// Calculate how many process identifiers were returned.  See article above.
	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.
	for (i = 0; i < cProcesses; i++) {
		if (aProcesses[i] != 0) {
			PrintProcessNameAndID(aProcesses[i]); }
	}
	printf("%ld processes found\n", i);

	return 0;
}
