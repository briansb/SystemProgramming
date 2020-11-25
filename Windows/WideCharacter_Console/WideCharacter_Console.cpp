// WideCharacter_Console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <wchar.h>


int wmain(int argc, wchar_t **argv) {
// or
// int wmain(void);
// int wmain(int argc, wchar_t *argv[]);

	// first calls wmainCRTStartup(), then calls wmain()

	//PDWORD cCharsWritten = NULL;  // pointer to a DWORD, which is a 32-bit unsigned integer.  
						   // typedef unsigned long DWORD;  
						   // typedef DWORD *PDWORD;
						   //   https://docs.microsoft.com/en-us/windows/win32/winprog/windows-data-types?redirectedfrom=MSDN
	// Don't know why they did that...it doesn't work

	// This does...
	DWORD cCharsWritten;

	HANDLE std = GetStdHandle(STD_OUTPUT_HANDLE);  // a handle (an abstract reference to a system resource, or iow, a souped-up pointer) to an object
												   // typedef PVOID HANDLE;
												   // PVOID - a pointer to any type
												   // typedef void *PVOID;
												   // STD_OUTPUT_HANDLE = -11, (DWORD), standard ouuput device

	if (std == INVALID_HANDLE_VALUE)		// INVALID_HANDLE_VALUE - return if GetStdHandle() fails
										    // GetLastError returns error code 
											// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
		{ wprintf(L"Cannot retrieve standard output handle\n (%d)", GetLastError()); }

	if (argv[1])		// first argument in command line
						// wcslen() - returns length of string
						// cChars - pointer to number of characters written
	{ WriteConsoleW(std, argv[1], wcslen(argv[1]), &cCharsWritten, NULL); }

	// can print cCharsWritten at this point

	CloseHandle(std);

	return 0;
}
