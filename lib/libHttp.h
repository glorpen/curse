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
