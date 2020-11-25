#include "pch.h"
#include "TCHAR.h"
#include "pdh.h"
#include <thread>
#include <chrono>
#include <iostream>

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;

void init() {
	PdhOpenQuery(NULL, NULL, &cpuQuery);
	// You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
	PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal); // percentage over all processors
	PdhCollectQueryData(cpuQuery);
}

double getCurrentValue() {
	PDH_FMT_COUNTERVALUE counterVal;

	PdhCollectQueryData(cpuQuery);
	PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
	return counterVal.doubleValue;
}

int main() {
	// see https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
	init();

	while (true) {

		std::cout << "CPU value = " << getCurrentValue() << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}