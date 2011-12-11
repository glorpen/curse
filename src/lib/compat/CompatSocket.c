/*
 *  Socket compatibility library for linux/windows.
 *  Copyright (C) 2011  Arkadiusz Dzięgiel
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common.h"
#include "CompatSocket.h"

#ifdef WIN32
	#include <winsock2.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <malloc.h>
	#include <unistd.h>
#endif

#define HOST_LOOKUP_ERROR 0

static uint32_t getHostAddr(char* host){
	struct hostent* h;
	h=gethostbyname(host);
	if(h==NULL){
		return HOST_LOOKUP_ERROR;
	}
	return *((uint32_t*)((h->h_addr_list)[0]));
}

void CSocketInitialize(){
	#ifdef WIN32
		WORD version = MAKEWORD(1,0);
		WSADATA wsaData;
		WSAStartup(version, &wsaData);
	#endif
}
void CSocketDestruct(){
	#ifdef WIN32
		WSACleanup();
	#endif
}

Socket CSocketConnect(char* host,uint16_t port){
	struct sockaddr_in saddr;
	int s=socket(AF_INET,SOCK_STREAM,0);

	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(port);
	saddr.sin_addr.s_addr=getHostAddr(host);
	if(saddr.sin_addr.s_addr==HOST_LOOKUP_ERROR)
		return SOCKET_ERROR;

	if(connect(s,(struct sockaddr*)&saddr,sizeof(struct sockaddr_in))==-1){
		return SOCKET_ERROR;
	}
	return s;
}

void CSocketClose(Socket s){
	#ifdef WIN32
		closesocket(s);
	#else
		close(s);
	#endif
}

uint64_t CSocketSend(int sockfd, const void *buf, uint64_t len){
	return send(sockfd, buf, len,0);
}
uint64_t CSocketRecv(int sockfd, void *buf, uint64_t len){
	return recv(sockfd, buf, len,0);
}

/*
int main(){
	CInitialize();
	Socket a=CConnect("google.pl",80);
	char* req="GET / HTTP/1.1\r\nHost: www.google.pl\r\n\r\n";
	char buf[1024];
	send(a,req,strlen(req),0);
	buf[recv(a,buf,150,0)]=0;
	printf("%s\n",buf);
	CClose(a);
	CDestruct();
	system("pause");
	return 0;
}
*/
