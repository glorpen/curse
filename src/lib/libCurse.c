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
 *  Created on: 05-12-2011
 *      Author: arkus
 */

#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#ifndef WIN32
	#include <ftw.h>
#else
	#include "lib/compat/gnu.h"
#endif

#include "common.h"
#include "lib/console.h"
#include "libDB.h"
#include "libCurse.h"
#include "libHttp.h"
#include <zip.h>

#define ADDON_PAGE "http://www.curse.com/addons/wow/%s"
#define ADDON_URL "http://www.curse.com/addons/wow/%s/%d"

static char* addons_dir = ".";
static char* db_path = "curse.db";

int32_t Curse_getRemoteVersion(const char* symbol){
	char url[strlen(symbol)+strlen(ADDON_PAGE)-1];
	char *pos, *pos_saved;
	int32_t ret = -1, status;

	sprintf(url, ADDON_PAGE, symbol);

	HttpStream hs;
	HttpInit(&hs);

	DEBUG("downloading page %s", url);
	HttpFillWithUrl(&hs, url);
	status=HttpRecvPage(&hs, NULL);
	if(status==HTTP_OK){
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
	} else {
		ERROR("could not download %s, error %d", url, status);
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
	DEBUG("version url for %s:%d - %s", symbol, version, url);
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
	int temp_fd=-1,ret;
	HttpStream hs;

	LOG("downloading... ");
	DEBUG("downloading %s", url);
	fflush(stdout);

	temp_fd=mkstemp(template);
	if(temp_fd==-1) {
		ERROR("could not create temp file");
		return -1;
	}

	HttpInit(&hs);
	HttpFillWithUrl(&hs, url);
	if((ret=HttpRecvPage(&hs, NULL))==HTTP_OK){
		output = fdopen(dup(temp_fd), "w");
		DEBUG("downloaded %ud bytes", hs.content_len);
		fwrite(hs.content, 1, hs.content_len, output);
		fclose(output);
	} else {
		ERROR("error downloading %s, error %d", url, ret);
		close(temp_fd);
	}

	HttpFree(&hs);
	return temp_fd;
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

#ifdef WIN32
#define cross_mkdir(x) mkdir(x)
#else
#define cross_mkdir(x) mkdir(x, S_IRWXU | S_IXGRP | S_IRWXO)
#endif

static bool recursive_mkdir(const char* root, const char* dir){
	int ret;
	char *p,*path=strdup(dir);
	char destination[512];

	p=path;

	while((p=strchr(p, '/'))!=NULL){
		p[0]=0;
		sprintf(destination, "%s/%s", root, path);
		ret=cross_mkdir(destination);
		p[0]='/';
		p++;

		if(ret == 0){
			DEBUG("created directory %s", destination);
		}
	}


	if(ret==-1 && errno != EEXIST){
		//error
		ERROR("could not create directory %s, error %d", destination, errno);
		free(path);
		return false;
	}

	return true;
}

static void unpackAddon(struct zip* archive){
	int i;
	zip_int64_t bytes_read;
	const char * zip_path;
	struct zip_file * file;
	char buff[1024], destination[512], *p;

	LOG("unpacking... ");

	for(i=0;i<zip_get_num_entries(archive, 0);i++){
		zip_path = zip_get_name(archive, i, 0);
		if(zip_path[strlen(zip_path)-1]=='/') continue;

		file = zip_fopen_index(archive, i, 0);
		DEBUG("reading %s", zip_path);

		sprintf(destination, "%s/%s", addons_dir, zip_path);

		p=strrchr(destination, '/');
		p[0]=0;

		if(recursive_mkdir(addons_dir, zip_path)){
			p[0]='/';

			FILE* out=fopen(destination, "wb");
			if(out==NULL){
				ERROR("could not write to %s", destination);
			} else {
				DEBUG("writing to %s", destination);
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

void Curse_update(char* symbol, bool force){
	int fd;
	int32_t remote_ver, local_ver;

	LOG("checking %s...  ", symbol);
	fflush(stdout);

	remote_ver = Curse_getRemoteVersion(symbol);
	local_ver = Curse_getLocalVersion(symbol);

	if(remote_ver < 0){
		LOG("addon not found");
	} else if(force || remote_ver > local_ver){
		LOG("updating addon to version %d... ", remote_ver);
		fd=Curse_downloadVersionFile(symbol, remote_ver);
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

void Curse_updateAll(bool force){
	DBObject* db=DBGetFirst();
	while(db != NULL){
		Curse_update(db->name, force);
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
	CSocketInitialize();
}

void Curse_free(){
	DBWrite(db_path);
	DBFree();
	CSocketDestruct();
}
