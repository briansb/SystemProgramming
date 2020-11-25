#ifndef SUPPORT_H
#define SUPPORT_H

#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>			/* needed for WINAPI, BOOL, etc */
#elif defined(__linux__)
	#include <unistd.h>
#endif

#include <stdio.h>			/* needed for printf() */
#include <signal.h>			/* keyboard control handler */

extern bool keep_running;

// OS-dependent routines
void winuxSetKeyboardHandler();



#endif
