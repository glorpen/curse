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

	int32_t ver = Curse_getRemoteVersion("clique");
	printf("%d : %d\n", ver, Curse_getLocalVersion("clique"));

	printf("download url: %s\n", Curse_versionDownloadUrl("clique" ,ver));

	Curse_free();

	return 0;
}
