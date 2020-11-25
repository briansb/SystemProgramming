// Classic_Console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <stdio.h>

int main()
// or
// int main(int argc, char **argv);
// int main(int argc, char *argv[]);
{
	// main() is an entry point, but not called first.
	// mainCRTStartup() is called first, and initializes memory, file I/O, etc.
    puts("This is a classic C program."); 
	
	return 0;
}
