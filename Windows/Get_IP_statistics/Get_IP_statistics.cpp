// Get_IP_statistics.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#ifndef UNICODE
#define UNICODE
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

/* Note: could also use malloc() and free() */
// https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getipstatistics?redirectedfrom=MSDN

int main()
{

	DWORD dwRetval;
	MIB_IPSTATS *pStats;

	pStats = (MIB_IPSTATS *)MALLOC(sizeof(MIB_IPSTATS));

	if (pStats == NULL) {
		wprintf(L"Unable to allocate memory for MIB_IPSTATS\n");
		exit(1);
	}
	dwRetval = GetIpStatistics(pStats);
	if (dwRetval != NO_ERROR) {
		wprintf(L"GetIpStatistics call failed with %d\n", dwRetval);
		exit(1);
	}
	else {

		wprintf(L"IP forwarding: \t\t");
		switch (pStats->dwForwarding) {
		case MIB_IP_FORWARDING:
			wprintf(L"Enabled\n");
			break;
		case MIB_IP_NOT_FORWARDING:
			wprintf(L"Disabled\n");
			break;
		default:
			wprintf(L"unknown value = %d\n", pStats->dwForwarding);
			break;
		}

		wprintf(L"Default initial TTL: \t\t\t\t\t%u\n", pStats->dwDefaultTTL);

		wprintf(L"Number of received datagrams: \t\t\t\t%u\n", pStats->dwInReceives);
		wprintf(L"Number of received datagrams with header errors: \t%u\n", pStats->dwInHdrErrors);
		wprintf(L"Number of received datagrams with address errors: \t%u\n", pStats->dwInAddrErrors);

		wprintf(L"Number of datagrams forwarded: \t\t\t\t%ld\n", pStats->dwForwDatagrams);

		wprintf(L"Number of received datagrams with an unknown protocol: \t%u\n", pStats->dwInUnknownProtos);
		wprintf(L"Number of received datagrams discarded: \t\t%u\n", pStats->dwInDiscards);
		wprintf(L"Number of received datagrams delivered: \t\t%u\n", pStats->dwInDelivers);

		wprintf(L"Number of outgoing datagrams requested to transmit: \t%u\n", pStats->dwOutRequests);
		wprintf(L"Number of outgoing datagrams discarded for routing: \t%u\n", pStats->dwRoutingDiscards);
		wprintf(L"Number of outgoing datagrams discarded: \t\t%u\n", pStats->dwOutDiscards);
		wprintf(L"Number of outgoing datagrams with no route to destination discarded: %u\n", pStats->dwOutNoRoutes);

		wprintf(L"Fragment reassembly timeout: \t\t\t\t%u\n", pStats->dwReasmTimeout);
		wprintf(L"Number of datagrams that required reassembly: \t\t%u\n", pStats->dwReasmReqds);
		wprintf(L"Number of datagrams successfully reassembled: \t\t%u\n", pStats->dwReasmOks);
		wprintf(L"Number of datagrams that could not be reassembled: \t%u\n", pStats->dwReasmFails);

		wprintf(L"Number of datagrams fragmented successfully: \t\t%u\n", pStats->dwFragOks);
		wprintf(L"Number of datagrams not fragmented and discarded: \t%u\n", pStats->dwFragFails);
		wprintf(L"Number of fragments created: \t\t\t\t%u\n", pStats->dwFragCreates);

		wprintf(L"Number of interfaces: \t\t\t\t\t%u\n", pStats->dwNumIf);
		wprintf(L"Number of IP addresses: \t\t\t\t%u\n", pStats->dwNumAddr);
		wprintf(L"Number of routes: \t\t\t\t\t%u\n", pStats->dwNumRoutes);
	}

	// Free memory allocated for the MIB_IPSTATS structure
	if (pStats)
		FREE(pStats);

	return 0;
}
