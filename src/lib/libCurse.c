/*
 * libCurse.c
 *
 *  Created on: 05-12-2011
 *      Author: arkus
 */

#define _XOPEN_SOURCE 500   /* See feature_test_macros(7) */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <ftw.h>

#include "common.h"
#include "libDB.h"
#include "libCurse.h"
#include "libHttp.h"
#include "lib/zip/zip.h"

#define ADDON_PAGE "http://www.curse.com/addons/wow/%s"
#define ADDON_URL "http://www.curse.com/addons/wow/%s/%d"

static char* addons_dir = ".";
static char* db_path = "curse.db";

int32_t Curse_getRemoteVersion(const char* symbol){
	char url[strlen(symbol)+strlen(ADDON_PAGE)-1];
	char *pos, *pos_saved;
	int32_t ret = -1;

	sprintf(url, ADDON_PAGE, symbol);

	HttpStream hs;
	HttpInit(&hs);

	HttpFillWithUrl(&hs, url);
	if(HttpRecvPage(&hs, NULL)==HTTP_OK){
		sprintf(url, "/addons/wow/%s/", symbol);

		pos = hs.content;
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
void Curse_setLocalVersion(char* symbol, int32_t version){
	DBObject* o;

	o = DBFind(symbol);
	if(o!=NULL){
		o->version = version;
	} else {
		DBPrepend(symbol, version);
	}
}

static char* getVersionDownloadUrl(const char* symbol, int32_t version){
	char url[strlen(ADDON_URL)+strlen(symbol)+20-2], *pos, *ret=NULL;
	sprintf(url, ADDON_URL, symbol, version);

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

int Curse_downloadVersionFile(char* symbol, int32_t version){
	char template[] = "/tmp/curseXXXXXX";
	char* url=getVersionDownloadUrl(symbol, version);
	FILE* output;
	int f=-1;
	HttpStream hs;

	DEBUG("downloading %s", url);

	HttpInit(&hs);
	HttpFillWithUrl(&hs, url);
	if(HttpRecvPage(&hs, NULL)==HTTP_OK){
		f=mkstemp(template);
		output = fdopen(dup(f), "w");
		DEBUG("writing %d\n", hs.content_len);
		fwrite(hs.content, 1, hs.content_len, output);
		fclose(output);
	}

	HttpFree(&hs);
	return f;
}

static int removeThing(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
	if(typeflag == FTW_DP){
		rmdir(fpath);
	} else {
		remove(fpath);
	}
	return 0;
}

static void deleteAddonDirs(struct zip* archive){
	char *path, *last_path=NULL, full_path[512];
	int i;

	for(i=0;i<zip_get_num_entries(archive, 0);i++){
		path = (char*)zip_get_name(archive, i, 0);
		path = strndup(path, strstr(path, "/")-path);

		if(last_path == NULL || strcmp(last_path, path)!=0){
			sprintf(full_path, "%s/%s", addons_dir, path);
			DEBUG("removing dir %s\n", full_path);
			if(nftw(full_path, removeThing, 4, 0)!=0){
				printf("could not remove directory %s, error %d\n", full_path, errno);
			}

		}
		if(last_path!=NULL) free(last_path);
		last_path = path;
	}
	if(last_path!=NULL) free(last_path);
}

static void unpackAddon(struct zip* archive){
	int i,ret;
	zip_int64_t bytes_read;
	const char * zip_path;
	struct zip_file * file;
	char buff[1024], destination[512], *p;

	for(i=0;i<zip_get_num_entries(archive, 0);i++){
		zip_path = zip_get_name(archive, i, 0);
		file = zip_fopen_index(archive, i, 0);
		printf("reading %s\n", zip_path);

		sprintf(destination, "%s/%s", addons_dir, zip_path);

		p=strrchr(destination, '/');
		p[0]=0;

		ret = mkdir(destination, S_IRWXU | S_IXGRP | S_IRWXO);
		if(ret==-1 && errno != EEXIST){
			printf("could not create directory %s, error %d\n", destination, errno);
		} else {
			p[0]='/';

			FILE* out=fopen(destination, "w");
			if(out==NULL){
				printf("could not write to %s\n", destination);
			} else {
				while((bytes_read = zip_fread(file, buff, 1024))>0){
					fwrite(buff, 1, bytes_read, out);
				}
				fclose(out);
			}
		}

		zip_fclose(file);
	}

}

bool Curse_install(int f){
	int error;
	struct zip* archive = zip_fdopen(f, 0, &error);

	if(archive==NULL){
		DEBUG("zip error %d\n", error);
		close(f);
		return false;
	} else {
		DEBUG("archive opened\n");
		deleteAddonDirs(archive);
		unpackAddon(archive);
	}
	zip_close(archive);

	return true;
}

void Curse_update(char* symbol){
	int fd;
	int32_t remote_ver = Curse_getRemoteVersion(symbol);
	int32_t local_ver = Curse_getLocalVersion(symbol);

	if(remote_ver > local_ver){
		fd=Curse_downloadVersionFile(symbol, remote_ver);
		if(fd!=-1 && Curse_install(fd)){
			Curse_setLocalVersion(symbol, remote_ver);
		}
	}
}

void Curse_init(char* working_dir){
	char path[512];

	sprintf(path, "%s/%s", working_dir, "Interface/AddOns");
	addons_dir = strdup(path);

	sprintf(path, "%s/%s", working_dir, "curse.db");
	db_path = strdup(path);

	DBRead(db_path);
}

void Curse_free(){
	DBWrite(db_path);
	DBFree();
}
