/*
 * main.c
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

int main(int argc, char** argv){

	ConsoleSetVerbosity(LOG_INFO);
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
		}
	}

	Curse_free();

	return 0;
}
