/*
 * gnu.h
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
