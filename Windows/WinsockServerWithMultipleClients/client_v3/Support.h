#ifndef SUPPORT_H
#define SUPPORT_H

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

#include <stdio.h>
#include <signal.h>     // keyboard control handler
#include <stdlib.h>

extern bool keep_running;

// OS-dependent routines
void zSetKeyboardHandler();



#endif
