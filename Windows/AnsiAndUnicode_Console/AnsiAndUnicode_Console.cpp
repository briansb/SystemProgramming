// AnsiAndUnicode_Console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"

//#define _UNICODE  // apparently, these are not needed
//#define UNICODE

#include <windows.h>
#include <tchar.h>

// again, have these options also:
// int _tmain(void);
// int _tmain(int argc, TCHAR **argv);
int _tmain(int argc, TCHAR *argv[]) {

	PDWORD cChars = NULL;
	HANDLE std = GetStdHandle(STD_OUTPUT_HANDLE);

	if (std == INVALID_HANDLE_VALUE) {
		_tprintf(L"Cannot retrieve standard output handle\n (%d)",
			GetLastError());
	}

	if (argv[1]) {

		WriteConsole(std, argv[1], _tcslen(argv[1]), cChars, NULL);  // note: using _tcslen instead of _wcslen
	}

	CloseHandle(std);

	return 0;
}


