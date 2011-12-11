/*
 * libCurse.h
 *
 *  Created on: 05-12-2011
 *      Author: arkus
 */

#ifndef LIBCURSE_H_
#define LIBCURSE_H_

int32_t Curse_getRemoteVersion(const char* symbol);
int32_t Curse_getLocalVersion(char* symbol);
void Curse_setLocalVersion(char* symbol, int32_t version);
int Curse_downloadVersionFile(char* symbol, int32_t version);
void Curse_setAddonDir(char* path);
void Curse_updateAll();
void Curse_update(char* symbol);

void Curse_free();
void Curse_init();

#endif /* LIBCURSE_H_ */
