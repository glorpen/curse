/*
 * main.c
 *
 *  Created on: 05-12-2011
 *      Author: arkus
 */

#include "common.h"
#include "lib/libDB.h"

int main(int argc, char** argv){

	/*
	DBRead("./curse.db");
	if(DBFind("asd")==NULL){
		printf("not found\n");
	}
	DBPrepend("asd",0);
	DBWrite("./curse.db");
	*/

	Curse_init();

	printf("%d : %d\n", Curse_getRemoteVersion("clique"), Curse_getLocalVersion("clique"));

	Curse_free();

	return 0;
}
