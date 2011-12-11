/*
 *  Socket compatibility header for linux/windows.
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
