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
	FILE* f;
	DBObject *current, *last=NULL, *first=NULL;
	uint32_t items,i;

	if((f=fopen(path, "r+b"))==NULL){
		if((f=fopen(path, "w+b"))==NULL)
			return NULL;
	}

	if(database!=NULL) DBFree();

	fseek(f, 0, SEEK_END);
	items = ftell(f)/(sizeof(uint32_t) + 32);
	fseek(f, 0, SEEK_SET);

	for(i=0;i<items;i++){
		current = DBObject_new();

		fread(current->name,32,1,f);
		fread(&(current->version),sizeof(uint32_t),1,f);

		if(first==NULL) first = current;
		if(last!=NULL) last->next = current;

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
