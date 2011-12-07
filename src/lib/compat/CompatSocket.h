#ifndef _CompatSocket
	#define _CompatSocket

	#include <stdint.h>

	typedef int Socket;
	#ifndef SOCKET_ERROR
		#define SOCKET_ERROR -1
	#endif

	extern uint64_t CSocketSend(int sockfd, const void *buf, uint64_t len);
	extern uint64_t CSocketRecv(int sockfd, void *buf, uint64_t len);
	extern void CSocketClose(Socket s);
	extern Socket CSocketConnect(char* host,uint16_t port);
	extern void CSocketDestruct();
	extern void CSocketInitialize();
#endif
