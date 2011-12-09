/*
 * main.c
 *
 *  Created on: 05-12-2011
 *      Author: arkus
 */

#include "common.h"

#include <unistd.h>

#include "lib/libDB.h"
#include "lib/libCurse.h"

void listItems(){
	DBObject* db=DBGetFirst();
	while(db != NULL){
		printf("\t%s\t%d\n", db->name, db->version);
		db = db->next;
	}
}

int main(int argc, char** argv){

	/*
	DBRead("./curse.db");
	if(DBFind("asd")==NULL){
		printf("not found\n");
	}
	DBPrepend("asd",0);
	DBWrite("./curse.db");
	*/

	if(argc==1){
		Curse_init(".");
	} else {
		Curse_init(argv[1]);

		if(argc>2){
			switch(argv[2][0]){
				case 'l': listItems(); break;
				case 's': Curse_setLocalVersion(argv[3], atoi(argv[4])); break;
				case 'r': DBRemove(argv[3]); break;
			}
		}
	}



	//int32_t ver = Curse_getRemoteVersion("clique");
	//printf("%d : %d\n", ver, Curse_getLocalVersion("clique"));

	//Curse_downloadFile("clique", ver, "/tmp/curse.zip.tmp");
	//Curse_update("bagnon");

	Curse_free();

	return 0;
}
