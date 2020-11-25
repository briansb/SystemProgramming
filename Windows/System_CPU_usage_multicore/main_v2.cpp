#include "pch.h"
#include "TCHAR.h"
#include "pdh.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <pdhmsg.h>


//#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	PDH_STATUS  pdhStatus = ERROR_SUCCESS;
	LPTSTR      szCounterListBuffer = NULL;
	DWORD       dwCounterListSize = 0;
	LPTSTR      szInstanceListBuffer = NULL;
	DWORD       dwInstanceListSize = 0;
	LPTSTR      szThisInstance = NULL;

	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("MyEvent"));

	// Determine the required buffer size for the data. 
	pdhStatus = PdhEnumObjectItems(
		NULL,                   // real time source
		NULL,                   // local machine
		TEXT("Processor"),        // object to enumerate
		szCounterListBuffer,    // pass NULL and 0
		&dwCounterListSize,     // to get length required
		szInstanceListBuffer,   // buffer size 
		&dwInstanceListSize,    // 
		PERF_DETAIL_WIZARD,     // counter detail level
		0);

	if (pdhStatus == PDH_MORE_DATA)
	{
		// Allocate the buffers and try the call again.
		szCounterListBuffer = (LPTSTR)malloc(
			(dwCounterListSize * sizeof(TCHAR)));
		szInstanceListBuffer = (LPTSTR)malloc(
			(dwInstanceListSize * sizeof(TCHAR)));

		if ((szCounterListBuffer != NULL) &&
			(szInstanceListBuffer != NULL))
		{
			pdhStatus = PdhEnumObjectItems(
				NULL,                 // real time source
				NULL,                 // local machine
				TEXT("Processor"),      // object to enumerate
				szCounterListBuffer,  // buffer to receive counter list
				&dwCounterListSize,
				szInstanceListBuffer, // buffer to receive instance list 
				&dwInstanceListSize,
				PERF_DETAIL_WIZARD,   // counter detail level
				0);

			if (pdhStatus == ERROR_SUCCESS)
			{
				_tprintf(TEXT("Enumerating Processor counters ... "));
				int nCount = 0;

				// Walk the instance list. The list can contain one
				// or more null-terminated strings. The last string 
				// is followed by a second null-terminator.
				for (szThisInstance = szInstanceListBuffer;
					*szThisInstance != 0;
					szThisInstance += lstrlen(szThisInstance) + 1)
				{
					// _tprintf (TEXT("\n  %s"), szThisInstance);
					if (0 != _tcscmp(szThisInstance, TEXT("_Total")))
					{
						// it's not the toalizer, so count it
						nCount++;
					}
				}

				_tprintf(TEXT("%d counters found\n"), nCount);

				HCOUNTER *hIdleCounters = new HCOUNTER[nCount];
				HCOUNTER *hPrivelegedCounters = new HCOUNTER[nCount];
				HCOUNTER *hProcessorCounters = new HCOUNTER[nCount];
				HQUERY hQuery;
				pdhStatus = PdhOpenQuery(NULL, 1, &hQuery);

				for (int n = 0; n < nCount; n++)
				{
					TCHAR szCounterPath[255];
					_stprintf_s(szCounterPath, 255, TEXT("\\Processor(%d)\\%% Processor Time"), n);
					pdhStatus = PdhAddCounter(hQuery,
						szCounterPath,
						n,
						&hProcessorCounters[n]);
					if (pdhStatus != ERROR_SUCCESS)
					{
						_tprintf(TEXT("Couldn't add counter \"%s\": 0x8.8X\n"),
							szCounterPath, pdhStatus);
						break;
					}

					_stprintf_s(szCounterPath, 255, TEXT("\\Processor(%d)\\%% Idle Time"), n);
					pdhStatus = PdhAddCounter(hQuery,
						szCounterPath,
						n,
						&hIdleCounters[n]);
					if (pdhStatus != ERROR_SUCCESS)
					{
						_tprintf(TEXT("Couldn't add counter \"%s\": 0x8.8X\n"),
							szCounterPath, pdhStatus);
						break;
					}

					_stprintf_s(szCounterPath, 255, TEXT("\\Processor(%d)\\%% Privileged Time"), n);
					pdhStatus = PdhAddCounter(hQuery,
						szCounterPath,
						n,
						&hPrivelegedCounters[n]);
					if (pdhStatus != ERROR_SUCCESS)
					{
						_tprintf(TEXT("Couldn't add counter \"%s\": 0x8.8X\n"),
							szCounterPath, pdhStatus);
						break;
					}
				}

				pdhStatus = PdhCollectQueryDataEx(hQuery, 2, hEvent);

				for (DWORD i = 0; i < 10; i++)
				{
					DWORD dwWaitResult = WaitForSingleObject(hEvent, INFINITE);
					if (WAIT_OBJECT_0 == dwWaitResult)
					{
						// first read always fails, since no previous sample.
						if (i == 0)
							continue;

						DWORD dwCounterType = 0;

						_tprintf(TEXT("Core    Idle  Kernel    Proc\n"));
						_tprintf(TEXT("---- ------- ------- -------\n"));

						for (int n = 0; n < nCount; n++)
						{
							PDH_FMT_COUNTERVALUE cvIdle, cvPriveleged, cvProcessor;
							pdhStatus = PdhGetFormattedCounterValue(hIdleCounters[n], PDH_FMT_DOUBLE, &dwCounterType, &cvIdle);
							if (pdhStatus != ERROR_SUCCESS)
							{
								_tprintf(TEXT("0: Error 0x%8.8X\n"), pdhStatus);
								break;
							}

							pdhStatus = PdhGetFormattedCounterValue(hPrivelegedCounters[n], PDH_FMT_DOUBLE, &dwCounterType, &cvPriveleged);
							if (pdhStatus != ERROR_SUCCESS)
							{
								_tprintf(TEXT("0: Error 0x%8.8X\n"), pdhStatus);
								break;
							}

							pdhStatus = PdhGetFormattedCounterValue(hProcessorCounters[n], PDH_FMT_DOUBLE, &dwCounterType, &cvProcessor);
							if (pdhStatus != ERROR_SUCCESS)
							{
								_tprintf(TEXT("0: Error 0x%8.8X\n"), pdhStatus);
								break;
							}

							_tprintf(TEXT("%-4d %#7.3f %#7.3f %#7.3f\n"), n, cvIdle.doubleValue, cvPriveleged.doubleValue, cvProcessor.doubleValue);
						}

						_tprintf(TEXT("\n"));
					}
				}

				delete[] hIdleCounters;
				delete[] hPrivelegedCounters;
				delete[] hProcessorCounters;

				PdhCloseQuery(hQuery);
			}
			else
			{
				_tprintf(TEXT("\nPdhEnumObjectItems failed with %ld."), pdhStatus);
			}
		}

		free(szCounterListBuffer);
		free(szInstanceListBuffer);

	}
	else
	{
		_tprintf(TEXT("\nPdhEnumObjectItems failed with %ld."), pdhStatus);
	}

	CloseHandle(hEvent);
	return 0;
}