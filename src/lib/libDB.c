#include "common.h"
#include "libDB.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

DBObject* database = NULL;

static DBObject* DBObject_new(){
	DBObject* o=malloc(sizeof(DBObject));
	o->name[0]=0;
	o->version=0;
	o->next=NULL;
	return o;
}

void DBFree(){
	DBObject *current=database, *last;
	while(current!=NULL){
		last = current;
		current = current->next;
		free(last);
	}

	database = NULL;
}

DBObject* DBRead(const char* path){
	FILE* f=fopen(path, "a+b");
	DBObject *current, *last=NULL, *first=NULL;

	if(f==NULL) return NULL;
	fseek(f, 0, SEEK_SET);
	if(database!=NULL){
		DBFree();
	}

	//TODO dont create empty object if no db

	while(!feof(f)){
		current = DBObject_new();

		if(first==NULL) first = current;

		fread(current->name,32,1,f);
		fread(&(current->version),sizeof(uint32_t),1,f);

		current->next = last;
		last = current;
	}
	fclose(f);

	database = first;

	return first;
}

bool DBWrite(const char* path){
	FILE* f=fopen(path, "wb");

	if(f==NULL){
		printf("could not save database %s\n", path);
		return false;
	}

	DBObject *current=database;
	while(current!=NULL){
		fwrite(current->name,32,1,f);
		fwrite(&(current->version),sizeof(uint32_t),1,f);

		current = current->next;
	}
	fclose(f);

	return true;
}

DBObject* DBFind(const char* name){
	DBObject* o=database;
	while(o!=NULL){
		if(strcmp(o->name, name)==0){
			return o;
		}
		o = o->next;
	}
	return NULL;
}

DBObject* DBPrepend(const char* name, uint32_t version){
	DBObject* n=DBObject_new();
	strcpy(n->name, name);
	n->version = version;
	n->next = database;
	database = n;
	return database;
}

DBObject* DBGetFirst(){
	return database;
}

void DBRemove(char* symbol){
	DBObject* o=database, *prev=NULL;
	while(o!=NULL){
		if(strcmp(o->name, symbol)==0){
			if(prev==NULL){ //first
				database = o->next;
			} else { //middle, last
				prev->next = o->next;
			}
			free(o);
			break;
		}

		prev = o;
		o = o->next;
	}
}
