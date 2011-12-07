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
static const char addon_url[] = "http://www.curse.com/addons/wow/%s/%d";
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

int32_t Curse_getLocalVersion(char* symbol){
	DBObject* o;

	o = DBFind(symbol);
	if(o!=NULL){
		return o->version;
	} else {
		return 0;
	}
}

char* Curse_versionDownloadUrl(const char* symbol, int32_t version){
	char url[strlen(addon_url)+strlen(symbol)+20-2], *pos, *ret=NULL;
	sprintf(url, addon_url, symbol, version);

	HttpStream hs;
	HttpInit(&hs);
	printf("%s\n", url);
	HttpFillWithUrl(&hs, url);
	if(HttpRecvPage(&hs, NULL)==HTTP_OK){
		pos = strstr(hs.content, "http://addons.curse.cursecdn.com/files");
		if(pos!=NULL){
			ret=pos;
			while(ret[0]!='"') ret++;
			ret[0]=0;
			ret=strdup(pos);
		}
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


