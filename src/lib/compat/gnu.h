/*
 *  GNU compatibility header for Mingw32.
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
 *      Author: arkus
 */

#ifndef GNU_H_
#define GNU_H_

#ifdef WIN32

#include <unistd.h>
#include <sys/stat.h>

extern int compat_mkstemp(char* template);
#define mkstemp(x) compat_mkstemp(x)

extern char *compat_strndup(const char *s, size_t n);
#define strndup(x,y) compat_strndup(x,y)


struct FTW {
};

enum
{
  FTW_F,                /* Regular file.  */
  FTW_D,                /* Directory.  */
  FTW_DP,               /* Directory, all subdirs have been visited. */
};

extern int compat_nftw(const char *dirpath, int (*fn) (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf), int nopenfd, int flags);
#define compat_nftw(a,b,c,d) nftw(a,b,c,d)

#endif

#endif /* GNU_H_ */
