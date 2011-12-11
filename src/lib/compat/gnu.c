/*
 *  GNU compatibility lib for Mingw32.
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
 *  Created on: 11-12-2011
 *      Author: Arkadiusz Dzięgiel
 */

#ifdef WIN32

#include "common.h"
#include "gnu.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>

int compat_mkstemp(char* template){
	FILE *f=tmpfile();
	if(f==NULL) return -1;
	return fileno(f);
}

char *compat_strndup(const char *s, size_t n){
	char buff[n+1];
	strncpy(buff, s, n);
	buff[n]=0;
	return strdup(buff);
}

#define NFTW_CALLBACK(x) int (*x) (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)

static void nftw_listDir(const char* dirpath, NFTW_CALLBACK(fn), int flags){
	char pattern[strlen(dirpath)+3];
	char buff[MAX_PATH];
	sprintf(pattern, "%s\\*", dirpath);

	WIN32_FIND_DATA ffd;
	HANDLE h=FindFirstFile(pattern, &ffd);

	if(h==INVALID_HANDLE_VALUE) return;
	do {
		if(strcmp(ffd.cFileName,".")==0 || strcmp(ffd.cFileName,"..")==0) continue;

		sprintf(buff, "%s\\%s", dirpath, ffd.cFileName);
		fn(buff, NULL,
			((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)?FTW_D:0) |
			((	(ffd.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) || (ffd.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) ||
				(ffd.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) || (ffd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
				)?FTW_F:0)
			,
			NULL
		);
		if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			nftw_listDir(buff, fn, flags);
			fn(buff, NULL, FTW_DP|FTW_D, NULL);
		}
	} while(FindNextFile(h, &ffd));

	FindClose(h);

}

int compat_nftw(const char *dirpath, NFTW_CALLBACK(fn), int nopenfd, int flags){
	nftw_listDir(dirpath, fn, flags);
	return 0;
}

#endif
