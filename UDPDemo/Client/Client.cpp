// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

const int MAX_PACK = 1500;
char buf[MAX_PACK];

#pragma comment(lib,"WS2_32.lib")

int main(int argc, char* argv[])
{
	if(argc	!= 4) {
		printf("usage: %s server port sleep\n",
			argv[0], argv[1], argv[2]);
		exit(0);
	}

	WSADATA wsadata;
	if (WSAStartup(WINSOCK_VERSION, &wsadata))
	{
	//	TRACE("Failed to Init Socket\n");
		WSACleanup();	
		exit(0);
	}

	char* server = argv[1];
	int port = atoi(argv[2]);
	int sleep = atoi(argv[3]);

	SOCKET sd;
	sd = socket(AF_INET, SOCK_DGRAM, 0);

	BOOL optval = TRUE;
	setsockopt(sd, SOL_SOCKET, SO_BROADCAST, (const char*)&optval, sizeof(optval));

	if(INVALID_SOCKET == sd) {
		printf("create socket fail\n");
		exit(0);
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(server);

	memset(buf, 9, MAX_PACK);

	for(;;)
	{
		memset(buf, 0, MAX_PACK);
		int n = sendto(sd, buf, MAX_PACK, 0, (const sockaddr*) & addr, sizeof(sockaddr_in));
		printf("sendto %d \n", n);
		Sleep(sleep);
	}

	WSACleanup();

	return 0;
}

