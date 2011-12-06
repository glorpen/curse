/*
 * libCurse.c
 *
 *  Created on: 05-12-2011
 *      Author: arkus
 */

#include "common.h"
#include "libDB.h"
#include "libCurse.h"
#include "libHttp.h"

#include <string.h>

static const char addon_page[] = "http://www.curse.com/addons/wow/%s";
static const char db_path[] = "curse.db";

int32_t Curse_getRemoteVersion(const char* symbol){
	char url[strlen(symbol)+strlen(addon_page)-1];
	int i;
	char *pos, *pos_saved;
	int32_t ret = -1;

	sprintf(url, addon_page, symbol);

	HttpStream hs;
	HttpInit(&hs);

	HttpFillWithUrl(&hs, url);
	if(HttpRecvPage(&hs, NULL)==HTTP_OK){
		sprintf(url, "/addons/wow/%s/", symbol);

		char* pos = hs.content;
		while(pos!=NULL){
			pos = strstr(pos, url);
			if(pos==NULL) break;
			pos+=strlen(url);
			pos_saved = pos;
			while(pos!=NULL && pos[0]<='9' && pos[0]>='0') pos++;
			if(pos==NULL) break;
			if(strncmp(pos, "-client", 7)==0){
				pos[0]=0;
				ret=atoi(pos_saved);
			}
		};
	}

	HttpFree(&hs);

	return ret;
}

void Curse_init(){
	DBRead(db_path);
}

void Curse_free(){
	DBFree();
}

int32_t Curse_getLocalVersion(char* symbol){
	DBObject* o;

	o = DBFind(symbol);
	if(o!=NULL){
		return o->version;
	} else {
		return 0;
	}
}
