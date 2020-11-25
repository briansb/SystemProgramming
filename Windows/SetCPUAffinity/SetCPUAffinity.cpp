// SetCPUAffinity.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include < Windows.h>
#include <thread>
// https://www.dynamsoft.com/blog/insights/set-cpu-affinity-process/
// https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
int main()
{
	const auto processor_count = std::thread::hardware_concurrency();
	std::cout << "This machine has " << processor_count << " processors" << std::endl;
	double sum;
	
	DWORD_PTR mask = 0x1;
	mask = mask << 2;

	//SetProcessAffinityMask(GetCurrentProcess(), 0x9/*CPU 1 and 4*/);
	SetProcessAffinityMask(GetCurrentProcess(), mask);
	while (1) {
		sum = 0;
		for (long i = 0; i < 100000000; ++i) {
			sum += i;
		}
		std::cout << "Sum = " << sum << std::endl;
	}
}
