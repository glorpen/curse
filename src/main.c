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

#include "common.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib/libDB.h"
#include "lib/libCurse.h"
#include "lib/console.h"

void listItems(){
	DBObject* db=DBGetFirst();
	printf("Addons list and their versions:\n");
	while(db != NULL){
		printf("%25s\t%d\n", db->name, db->version);
		db = db->next;
	}
}

void zero(){
	DBObject* db=DBGetFirst();
	printf("Clearing versions...\n");
	while(db != NULL){
		db->version=0;
		db = db->next;
	}
}

int main(int argc, char** argv){

	ConsoleSetVerbosity(LOG_WARNING);
	Curse_init(".");

	if(argc>1){
		if(strcmp("list", argv[1])==0){
			listItems();
		} else if(strcmp("set", argv[1])==0){
			Curse_setLocalVersion(argv[2], atoi(argv[3]));
		} else if(strcmp("remove", argv[1])==0){
			DBRemove(argv[2]);
		} else if(strcmp("update", argv[1])==0){
			Curse_updateAll();
		} else if(strcmp("zero", argv[1])==0){
			zero();
		}
	}

	Curse_free();

	return 0;
}
