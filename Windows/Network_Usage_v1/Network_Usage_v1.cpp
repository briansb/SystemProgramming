// Network_Usage_v1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "pdh.h"

#include <iostream>
#include <chrono>
#include <thread> 
#include <string>


// this might come in handy...don't have to change development environment
// #pragma comment(lib, "pdh.lib")

static PDH_HQUERY myQuery;

static PDH_HCOUNTER BytesReceivedCounter;
static PDH_HCOUNTER BytesSentCounter;
static PDH_HCOUNTER BytesTotalCounter;

int main() {
	
	PDH_FMT_COUNTERVALUE BytesReceivedCounterValue;
	PDH_FMT_COUNTERVALUE BytesSentCounterValue;
	PDH_FMT_COUNTERVALUE BytesTotalCounterValue;

	PdhOpenQuery(NULL, NULL, &myQuery);
	
	PdhAddCounter(myQuery, L"\\Network Interface(*)\\Bytes Received/sec", NULL, &BytesReceivedCounter);
	PdhAddCounter(myQuery, L"\\Network Interface(*)\\Bytes Sent/sec", NULL, &BytesSentCounter);
	PdhAddCounter(myQuery, L"\\Network Interface(*)\\Bytes Total/sec", NULL, &BytesTotalCounter);


	PdhCollectQueryData(myQuery);

	bool keep_running = true;
	while (keep_running || std::cin.get() ) {
		
		PdhCollectQueryData(myQuery);

		PdhGetFormattedCounterValue(BytesReceivedCounter, PDH_FMT_DOUBLE, NULL, &BytesReceivedCounterValue);
		std::cout << "Bytes received = " << BytesReceivedCounterValue.doubleValue << std::endl;
		PdhGetFormattedCounterValue(BytesSentCounter, PDH_FMT_DOUBLE, NULL, &BytesSentCounterValue);
		std::cout << "Bytes sent = " << BytesSentCounterValue.doubleValue << std::endl;
		PdhGetFormattedCounterValue(BytesTotalCounter, PDH_FMT_DOUBLE, NULL, &BytesTotalCounterValue);
		std::cout << "Bytes total = " << BytesTotalCounterValue.doubleValue << std::endl;


		std::cout << "Sleep for one seconds" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << std::endl;
		keep_running = true;  // set this false to run one loop at a time
	}

	return 0;
}