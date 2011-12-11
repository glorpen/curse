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
 */

#ifndef __LIB_DB__
#define __LIB_DB__

typedef struct _DBObject {

	char name[32];
	uint32_t version;

	struct _DBObject* next;
} DBObject;

extern void DBFree();
extern DBObject* DBRead(const char* path);
extern bool DBWrite(const char* path);
extern DBObject* DBFind(const char* name);
extern DBObject* DBPrepend(const char* name, uint32_t version);
extern DBObject* DBGetFirst();
extern void DBRemove(char* symbol);

#endif
