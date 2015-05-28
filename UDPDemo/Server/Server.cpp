// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h>

#pragma comment(lib,"WS2_32.LIB")

const int MAX_PACK = 4000;
char buf [MAX_PACK];

int main(int argc, char* argv[])
{
	
	WSADATA wsadata;
	char* server;
	int port;

	if (WSAStartup(WINSOCK_VERSION, &wsadata))
	{
		WSACleanup();	
		exit(0);
	}
	
	if(argc != 3) 
	{
		printf("usage: %s server port \n", argv[0]);
		server = NULL;
		port = 8004;
	}else{
		server = argv[1];
		port = atoi(argv[2]);
		printf("%s %d\n", server, port);
	}

	SOCKET sd;

	sd = socket(AF_INET, SOCK_DGRAM, 0);

	if(sd == INVALID_SOCKET)
	{
		printf("create sock fail\n");
		exit(0);
	}

	sockaddr_in name;
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.S_un.S_addr = 0;//server?inet_addr(server):0;


	if(bind(sd, (const sockaddr*) &name, sizeof(sockaddr_in)) != 0) {
		printf("bind fail \n");
		exit(0);
	}

	printf(" bind %d\n",port);
	for(;;)
	{
		sockaddr_in client;
		fd_set rset;
		
		int len = sizeof(sockaddr_in);
		FD_ZERO(&rset);
		FD_SET(sd, &rset);
		int n = select(0,&rset,NULL,NULL,NULL);
		if (n > 0) {
			printf("select return %d\n", n);
			n = recvfrom(sd, buf, MAX_PACK, 0, (sockaddr*) & client, & len);
			printf("recv %d %s\n", n,inet_ntoa(client.sin_addr));
			printf("%x %x %x %x\n", buf[0],buf[1],buf[2],buf[3]);
		}
	}

	WSACleanup();    
	return 0;
}

struct packet_head { 
	DWORD	 flags;					
	WORD	 seq;																								
	BYTE	 type;				
	BYTE   cmd;   
	DWORD	 len;		
};

struct media_video_info{
	DWORD timestamp;       
	DWORD len;       
	char  head[15]; //cam_id "nyang123456789"
	char  type;
};


void recv_video(char* buf, int len)
{
	static char media_buf[1024*1024];
	static int offset = 0;
	static int frame = -1;
	static int num = 0;

	packet_head hdr;
	media_video_info media;
	
	memcpy(&hdr,buf,sizeof(packet_head));
	memcpy(&media,buf,sizeof(media_video_info));
	
	if (hdr.seq > frame) {
		//new one
		offset = 0;
		memcpy(media_buf,buf+sizeof(hdr)+sizeof(media), hdr.len - sizeof(media));
		offset += (hdr.len - sizeof(media));
	} else if (hdr.seq == frame){
				
	} else {
		printf("sorry,late coming one, discard\n");
	}
}