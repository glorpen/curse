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

#ifndef LIBCURSE_H_
#define LIBCURSE_H_

extern int32_t Curse_getRemoteVersion(const char* symbol);
extern int32_t Curse_getLocalVersion(char* symbol);
extern void Curse_setLocalVersion(char* symbol, int32_t version);
extern int Curse_downloadVersionFile(char* symbol, int32_t version);
extern void Curse_setAddonDir(char* path);
extern void Curse_updateAll();
extern void Curse_update(char* symbol);

extern bool Curse_install(int f);

extern void Curse_free();
extern void Curse_init();

#endif /* LIBCURSE_H_ */
