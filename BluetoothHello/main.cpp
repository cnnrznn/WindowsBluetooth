#pragma comment(lib, "Ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2bth.h>
#include <stdio.h>

int main() 
{
	SOCKADDR_BTH laddr, raddr; /* local and remote socket addresses */
	SOCKET sock = INVALID_SOCKET;
	WSAQUERYSET querySet;
	HANDLE handle;
	int flags = LUP_RETURN_NAME | LUP_RETURN_ADDR;

	// set query options
	memset(&querySet, 0, sizeof(querySet));
	querySet.dwSize = sizeof(querySet);
	querySet.dwNameSpace = NS_BTH;
	querySet.lpBlob = NULL;

	// clear address memory
	memset(&laddr, 0, sizeof(laddr));
	memset(&raddr, 0, sizeof(raddr));

	// 1. START WSA SOCKETS
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
		printf("Error calling WSAStartup()\n");
		exit(1);
	}

	// 2. DISCOVER SERVICE
	// allocate buffer
	CHAR buf[4096];
	LPWSAQUERYSET queryResults = (LPWSAQUERYSET)buf;
	memset(queryResults, 0, sizeof(buf));
	queryResults->dwSize = sizeof(WSAQUERYSET);
	queryResults->dwNameSpace = NS_BTH;
	DWORD dwSize = sizeof(buf);
	
	printf("Discovering services...\n");
	if (ERROR_SUCCESS != WSALookupServiceBegin(&querySet, LUP_CONTAINERS | LUP_FLUSHCACHE, &handle)) {
		printf("Error calling WSALookupServiceBegin(): %d\n", WSAGetLastError());
	}
	if (ERROR_SUCCESS == WSALookupServiceNext(handle, flags, &dwSize, queryResults)) {
		printf("First device found:\n");
		printf("Service: %s\n", (CHAR *)queryResults->lpszServiceInstanceName);
		raddr = *(SOCKADDR_BTH *)queryResults->lpcsaBuffer->RemoteAddr.lpSockaddr;
		printf("Address: %d\n", raddr.btAddr);
		printf("Address: 0x%X\n", raddr.btAddr);
		printf("af == af_bth?: %d\n", raddr.addressFamily == AF_BTH);
		printf("Port: %d\n", raddr.port);
	}
	WSALookupServiceEnd(handle);

	// 3. OPEN A SOCKET TO SERVICE
	printf("Creating a bluetooth socket...\n");
	sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

	// 4. COMMUNICATE
	// TODO

	// 5. TERMINATE
	closesocket(sock);
	WSACleanup();

	printf("Press any key to continue...");
	getc(stdin);

	return 0;
}
