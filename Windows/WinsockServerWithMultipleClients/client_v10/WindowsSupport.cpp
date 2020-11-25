#include "Support.h"

// These are Windows-specific routines
#if defined(_WIN32) || defined(_WIN64)

// https://docs.microsoft.com/en-us/windows/console/registering-a-control-handler-function
// https://docs.microsoft.com/en-us/windows/console/setconsolectrlhandler
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
	switch (fdwCtrlType) {
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		Beep(750, 300);
		keep_running = false;
		return TRUE;
		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		printf("Ctrl-Break event\n\n");
		Beep(900, 200);
		keep_running = false;
		return TRUE;
	default:
		return FALSE;
	}
}
void winuxSetKeyboardHandler() {
	SetConsoleCtrlHandler(CtrlHandler, TRUE);
}




#endif