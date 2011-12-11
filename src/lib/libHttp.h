/*
 *  Curse, World of Warcraft addons updater.
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

#ifndef _MyHTTP
	#define _MyHTTP

#include <stdint.h>
#include "compat/CompatSocket.h"

#define HTTP_OK            0
#define HTTP_CONNECT_ERROR 1
#define HTTP_REQUEST_ERROR 2
#define HTTP_HEADERS_ERROR 3
#define HTTP_BODY_ERROR    4

typedef struct {
	char* host;
	uint16_t port;
	char* path;

	Socket socket;
	char*** headers;
	uint16_t status;
	char* content;
	uint32_t content_len;
	bool followRedirect;
} HttpStream;

extern char* GetHttpHeaderValue(HttpStream* hs,char* key);
extern uint8_t HttpRecvPage(HttpStream* hs, char* postData);
extern void HttpInit(HttpStream* hs);
extern void HttpFree(HttpStream* hs);
extern void HttpFillWithUrl(HttpStream* hs, char*url);
extern char* urlencode(char*str);

#endif
