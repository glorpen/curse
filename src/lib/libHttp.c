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

#include "common.h"
#include "libHttp.h"
#include "compat/CompatSocket.h"

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <zlib.h>
static char* common_req=" %s HTTP/1.1\r\nAccept-Encoding: gzip,deflate,identity\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US; rv:1.9b5) Gecko/2008032620 Firefox/3.1\r\nHost: %s\r\nConnection: close\r\n\r\n";
static char* get_req="GET%s";
static char* post_req="POST%s%s";

static bool makeHttpRequest(HttpStream* hs, char* postData){
	char *req,*common;
	uint32_t req_len=strlen(common_req)+strlen(hs->host)+strlen(hs->path)-4;
	common=malloc(req_len+1);
	sprintf(common,common_req,hs->path,hs->host);
	
	if(postData==NULL){
		req_len+=strlen(get_req)-2;
		req=malloc(req_len+1);
		sprintf(req,get_req,common);
	} else {
		req_len+=strlen(post_req)+strlen(postData)-2;
		req=malloc(req_len+1);
		sprintf(req,post_req,common,postData);
	}
	free(common);
	CSocketSend(hs->socket,req,req_len);
	free(req);
	return true;
}
static void toLower(char*str){
	uint16_t l=strlen(str),i;
	for(i=0;i<l;i++){
		if(str[i]>64 && str[i]<91){
			str[i]+=32;
		}
	}
}
static bool parseHeaders(HttpStream* hs,char* headers){
	/*get number of lines*/
	int16_t lines=-2,i;
	char* pos=headers;
	char *tmp,*tmp2;

	while((pos=strstr(pos,"\r\n"))!=NULL){
		lines++;
		pos+=2;
	}
	sscanf(headers,"HTTP/%*f %hu",&(hs->status));

	hs->headers=malloc(sizeof(char**)*(lines+1));

	pos=headers;
	i=0;
	while((pos=strstr(pos,"\r\n"))!=NULL){
		hs->headers[i]=malloc(sizeof(char*)*2);
		pos+=2;
		tmp=strchr(pos,':');
		tmp[0]=0;
		hs->headers[i][0]=strdup(pos);
		toLower(hs->headers[i][0]);
		tmp[0]=':';
		tmp+=2;
		tmp2=strstr(tmp,"\r\n");
		tmp2[0]=0;
		hs->headers[i][1]=strdup(tmp);
		tmp2[0]='\r';

		lines--;i++;
		if(lines==0) break;
	}
	hs->headers[i]=NULL;
	return true;
}

char* GetHttpHeaderValue(HttpStream* hs,char* key){
	uint8_t i=0;
	if(hs->headers!=NULL)
	while(hs->headers[i]!=NULL){
		if(strcmp(hs->headers[i][0],key)==0){
			return hs->headers[i][1];
		}
		i++;
	}
	return NULL;
}

static bool getHttpHeaders(HttpStream* hs){
	char* buf=malloc(1024);
	uint16_t bl=0;

	uint8_t c;
	uint8_t oks=0;
	char* end="\r\n\r\n";

	while(CSocketRecv(hs->socket,&c,1)>0){
		buf[bl++]=c;
		if(bl%1024==0){buf=realloc(buf,bl+1024);}
		if(end[oks]==c) {
			oks++;
			if(oks==4) break;
			continue;
		} else {oks=0;}
	}
	buf[bl]=0;

	parseHeaders(hs,buf);
	free(buf);
	if(oks==4) return true;
	return false;
}

static bool readHttpData(HttpStream* hs,uint32_t len){
	int32_t ret;
	uint32_t start=hs->content_len;
	hs->content=realloc(hs->content,start+len+1);
	while(hs->content_len!=len+start){
		ret=CSocketRecv(hs->socket,(hs->content)+start+hs->content_len,len-hs->content_len);
		if(ret<1) return false;
		hs->content_len+=ret;
	}
	hs->content[start+len]=0;
	return true;
}

static bool getHttpChunked(HttpStream*hs){
	char* br="\r\n";
	char buf1[10];
	uint8_t c;
	uint32_t i,j,chunk_len;

	i=0;j=0;
	while(CSocketRecv(hs->socket,&c,1)>0){
		if(br[i]==c){
			i++;
			if(i==2){
				buf1[j-1]=0;
				chunk_len=0;
				sscanf(buf1,"%x",&chunk_len);
				if(chunk_len==0) return true;
				readHttpData(hs,chunk_len);
				i=0;j=0;
			}
		} else i=0;
		buf1[j++]=c;
	}
	return false;
}

static bool zlibDecode(z_stream* stream,HttpStream*hs){
	char* buff=malloc(1025);
	uint32_t buff_len=1024;

	stream->avail_out=1024;
	stream->next_out=(Bytef*)buff;

	while(stream->avail_in>0){
		if(inflate(stream,Z_NO_FLUSH)!=Z_OK) break;
		if(stream->avail_out==0){
			buff_len+=1024;
			buff=realloc(buff,buff_len+1);
			stream->avail_out=1024;
			stream->next_out=(Bytef*)(buff+(buff_len-1024));
		}
	}
	buff[stream->total_out]=0;
	free(hs->content);
	hs->content=buff;
	hs->content_len=stream->total_out;
	inflateEnd(stream);
	return true;
}

static bool httpGzipDecode(HttpStream*hs){ //+10
	z_stream stream;
	stream.zalloc=Z_NULL;
	stream.zfree=Z_NULL;
	stream.opaque=Z_NULL;

	stream.avail_in=hs->content_len-10;
	stream.next_in=(Bytef*)(hs->content+10);

	if(inflateInit2(&stream,-15)!=Z_OK) {return false;}

	return zlibDecode(&stream,hs);
}
static bool httpDeflateDecode(HttpStream*hs){
	z_stream stream;
	stream.zalloc=Z_NULL;
	stream.zfree=Z_NULL;
	stream.opaque=Z_NULL;

	stream.avail_in=hs->content_len;
	stream.next_in=(Bytef*)hs->content;

	if(inflateInit(&stream)!=Z_OK) {return false;}

	return zlibDecode(&stream,hs);
}

static bool getHttpBody(HttpStream* hs){
	char* len_s=GetHttpHeaderValue(hs,"content-length");
	char* encoding=GetHttpHeaderValue(hs,"content-encoding");
	uint32_t len;

	if(len_s==NULL){
		if(!getHttpChunked(hs))
			return false;
	} else {
		sscanf(len_s,"%u",&len);
		if(!readHttpData(hs,len)) return false;
	}
	if(encoding!=NULL){
		if(strcmp(encoding,"gzip")==0){
			httpGzipDecode(hs);
		}
		if(strcmp(encoding,"deflate")==0){
			httpDeflateDecode(hs);
		}
	}

	return true;
}

static uint8_t HttpRecv(HttpStream* hs, char* postData){
	hs->socket=CSocketConnect(hs->host,hs->port);
	if(hs->socket==SOCKET_ERROR)
		return HTTP_CONNECT_ERROR;
	if(!makeHttpRequest(hs,NULL)){
		CSocketClose(hs->socket);
		return HTTP_REQUEST_ERROR;
	};
	if(!getHttpHeaders(hs)){
		CSocketClose(hs->socket);
		return HTTP_HEADERS_ERROR;
	};
	if(!getHttpBody(hs)){
		CSocketClose(hs->socket);
		return HTTP_BODY_ERROR;
	};
	CSocketClose(hs->socket);
	return HTTP_OK;
}

static uint8_t HttpRecvFollow(HttpStream* hs, char* postData){
	uint8_t ret;
	char* url;
	while(1){
		ret=HttpRecv(hs,postData);
		if(ret!=HTTP_OK) return ret;
		switch(hs->status){
			case 301:
			case 302:
				url=strdup(GetHttpHeaderValue(hs,"location"));
				HttpFree(hs);
				HttpInit(hs);
				HttpFillWithUrl(hs,url);
				break;
			default:
				return ret;
		}
	}
}

uint8_t HttpRecvPage(HttpStream* hs, char* postData){
	if(hs->followRedirect) return HttpRecvFollow(hs,postData);
	return HttpRecv(hs,postData);
}

void HttpInit(HttpStream* hs){
	hs->host=NULL;
	hs->path=NULL;
	hs->headers=NULL;
	hs->content=NULL;
	hs->content_len=0;
	hs->status=0;
	hs->port=0;
	hs->followRedirect=true;
}
void HttpFree(HttpStream* hs){
	uint8_t i=0;

	free(hs->host);
	free(hs->path);
	free(hs->content);
	if(hs->headers!=NULL){
		while(hs->headers[i]!=NULL){
			free(hs->headers[i][0]);
			free(hs->headers[i][1]);
			free(hs->headers[i]);
			i++;
		}
		free(hs->headers);
	}
}

void HttpFillWithUrl(HttpStream* hs, char*url){
	char*p,*tmp;
	char*host,*path;
	uint16_t port=80;

	if(strncmp(url,"http://",7)==0) {
		p=strdup(url+7);
	} else {
		p=strdup(url);
	}
	tmp=strchr(p,'/');
	if(tmp==NULL){
		host=strdup(p);
		path=strdup("/");
	} else {
		path=strdup(tmp);
		tmp[0]=0;
		host=strdup(p);
	}
	free(p);
	tmp=strchr(host,':');
	if(tmp!=NULL){
		tmp[0]=0;
		sscanf(tmp+1,"%hu",&port);
	}
	hs->host=host;
	hs->port=port;
	hs->path=path;
}

char* urlencode(char*str){
	uint16_t len=strlen(str)+1,i,l=0;
	char* out=NULL;

	for(i=0;i<len;i++){
		if(l%10==0) {l+=10;out=realloc(out,l);}
		switch(str[i]){
			case ' ':
				out[i]='+';
				break;
			default:
				out[i]=str[i];
				break;
		}
		l++;
	}
	return out;
}

/*int main(){
	uint8_t ret;
	HttpStream s;
	CInitialize();
	HttpInit(&s);
	HttpFillWithUrl(&s,"http://arkus.malopolska.pl");
	ret=HttpRecv(&s,NULL);
	switch(ret){
		case HTTP_CONNECT_ERROR:
			printf("cant connect to host\n");break;
	}
	printf("%s\n",s.content);
	HttpFree(&s);
	CDestruct();
	return 0;
}
*/
