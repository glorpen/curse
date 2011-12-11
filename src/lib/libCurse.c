/*
 * libCurse.c
 *
 *  Created on: 05-12-2011
 *      Author: arkus
 */

#define _XOPEN_SOURCE 500   /* See feature_test_macros(7) */
#define _GNU_SOURCE

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
#include "lib/console.h"
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
	DEBUG("version url for %s:%d - %s\n", symbol, version, url);
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
	int f=-1,ret;
	HttpStream hs;

	LOG("downloading... ");
	DEBUG("downloading %s", url);
	fflush(stdout);

	HttpInit(&hs);
	HttpFillWithUrl(&hs, url);
	if((ret=HttpRecvPage(&hs, NULL))==HTTP_OK){
		f=mkstemp(template);
		output = fdopen(dup(f), "w");
		DEBUG("writing %d", hs.content_len);
		fwrite(hs.content, 1, hs.content_len, output);
		fclose(output);
	} else {
		ERROR("error downloading %s, error %d", url, ret);
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

	LOG("deleting old files... ");

	for(i=0;i<zip_get_num_entries(archive, 0);i++){
		path = (char*)zip_get_name(archive, i, 0);
		path = strndup(path, strstr(path, "/")-path);

		if(last_path == NULL || strcmp(last_path, path)!=0){
			sprintf(full_path, "%s/%s", addons_dir, path);
			DEBUG("removing dir %s", full_path);
			if(nftw(full_path, removeThing, 4, 0)!=0){
				WARNING("could not remove directory %s, error %d", full_path, errno);
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

	LOG("unpacking... ");

	for(i=0;i<zip_get_num_entries(archive, 0);i++){
		zip_path = zip_get_name(archive, i, 0);
		if(zip_path[strlen(zip_path)-1]=='/') continue;

		file = zip_fopen_index(archive, i, 0);
		DEBUG("reading %s\n", zip_path);

		sprintf(destination, "%s/%s", addons_dir, zip_path);

		p=strrchr(destination, '/');
		p[0]=0;

		ret = mkdir(destination, S_IRWXU | S_IXGRP | S_IRWXO);
		if(ret==-1 && errno != EEXIST){
			ERROR("could not create directory %s, error %d", destination, errno);
		} else {
			p[0]='/';

			FILE* out=fopen(destination, "w");
			if(out==NULL){
				ERROR("could not write to %s", destination);
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
		ERROR("zip error %d", error);
		close(f);
		return false;
	} else {
		DEBUG("archive opened");
		deleteAddonDirs(archive);
		unpackAddon(archive);
	}
	zip_close(archive);

	return true;
}

void Curse_update(char* symbol){
	int fd;
	int32_t remote_ver, local_ver;

	LOG("checking %s...  ", symbol);
	fflush(stdout);

	remote_ver = Curse_getRemoteVersion(symbol);
	local_ver = Curse_getLocalVersion(symbol);

	if(remote_ver > local_ver){
		LOG("updating addon...");
		INFO("found remote version %d", remote_ver);
		fd=Curse_downloadVersionFile(symbol, remote_ver);
		fflush(stdout);
		if(fd!=-1 && Curse_install(fd)){
			Curse_setLocalVersion(symbol, remote_ver);
			LOG("OK");
		} else {
			LOG("ERROR");
		}
	} else {
		LOG("OK");
	}
}

void Curse_updateAll(){
	DBObject* db=DBGetFirst();
	while(db != NULL){
		Curse_update(db->name);
		db = db->next;
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
