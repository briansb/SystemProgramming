// Process_CPU_Usage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <Windows.h>



int main()
{
	// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getprocesstimes
	// https://docs.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-filetime
	// https://stackoverflow.com/questions/43065154/getprocesstimes-on-windows-is-imprecise
	while (true) {

		FILETIME user, kernel, exit, start;
		ULARGE_INTEGER userCPU, kernelCPU;

		/*HANDLE hProcess;
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, 5996);
		bool res = GetProcessTimes(hProcess, &start, &exit, &kernel, &user);*/
		// or ....
		bool res = GetProcessTimes(GetCurrentProcess(), &start, &exit, &kernel, &user);

		userCPU.LowPart = user.dwLowDateTime;
		userCPU.HighPart = user.dwHighDateTime;
		kernelCPU.LowPart = kernel.dwLowDateTime;
		kernelCPU.HighPart = kernel.dwHighDateTime;

		double j;
		for (double i = 0; i < 10000000; ++i) {
			j = i * i / 2 * i / i;
		}

		//*pUser = (unsigned long long)userCPU.QuadPart;
		//*pKernel = (unsigned long long)kernelCPU.QuadPart;
		std::cout << start.dwHighDateTime << " - " << start.dwLowDateTime << std::endl;
		std::cout << exit.dwHighDateTime << " - " << exit.dwLowDateTime << std::endl;
		std::cout << kernel.dwHighDateTime << " - " << kernel.dwLowDateTime << std::endl;
		std::cout << user.dwHighDateTime << " - " << user.dwLowDateTime << std::endl;
		std::cout << "Quad - " << userCPU.QuadPart / 10000 << std::endl;

		std::cout << std::endl;

		// Do stuff which takes longer than a millisecond
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		
	}

}