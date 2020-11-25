// WinMain_skeleton.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>

// additional:
// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
// int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
// int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR szCmdLine, int CmdShow);

#undef UNICODE
#define UNICODE

int wmain(int argc, wchar_t* argv[])
{
	MessageBox(0, L"Press OK", argv[1], MB_SETFOREGROUND);  //https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox

	return 0;

}