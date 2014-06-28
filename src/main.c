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
#include <getopt.h>

#include "lib/libDB.h"
#include "lib/libCurse.h"
#include "lib/console.h"

static void listItems(){
	DBObject* db=DBGetFirst();
	printf("Addons list and their versions:\n");
	if(db == NULL)
		printf("\t-- no addons found --\n");
	while(db != NULL){
		printf("%25s\t%d\n", db->name, db->version);
		db = db->next;
	}
}

static void zero(){
	DBObject* db=DBGetFirst();
	printf("Clearing versions...\n");
	while(db != NULL){
		db->version=0;
		db = db->next;
	}
}

static void help(){
	printf("\
Usage:\n\
\t-h, --help                   this text\n\
\t-v, --verbose                  make output more verbose (can be used multiple times)\n\
\t-s, --set <name> <version>     sets addon version in database\n\
\t-r, --remove <name>            removes addon from database\n\
\t-c, --clear                    clears (zeroes) versions of all addons\n\
\t-u[name], --update=[name]      checks for updates for all addons or one given by name\n\
\t-u,--update=[name] -f|--force  reinstalls addon\n\
\t-d, --dir <dir>                WoW instalation folder, default: current directory\n\
");
}

int main(int argc, char** argv){
	void* fun = NULL;
	int c;
	uint8_t verbosity = LOG_ERROR;
	char* dir=strdup(".");
	char* addon_to_update = NULL;
	bool force_update = false;

	printf("Curse - an addon uddater for World of Warcraft.\n\n");

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
				{ "help", no_argument, 0, 'h' },
				{ "verbose", no_argument, 0, 'v' },
				{ "list", no_argument, 0, 'l' },
				{ "set", no_argument, 0, 's' },
				{ "remove", no_argument, 0, 'r' },
				{ "force", no_argument, 0, 'f' },
				{ "clear", no_argument, 0, 'c' },
				{ "update", optional_argument, 0, 'u' },
				{ "dir", required_argument, 0, 'd' },
				{ 0, 0, 0, 0 } };

		c = getopt_long(argc, argv, "fhvsrcu::ld:", long_options, &option_index);
		if (c == -1) break;

		switch (c) {
			case 'h': fun = help; break;
			case 'l': fun = listItems; break;
			case 's': fun = Curse_setLocalVersion; break;
			case 'r': fun = DBRemove; break;
			case 'u':
				if(optarg){
					fun = Curse_update;
					addon_to_update = strdup(optarg);
				} else {
					fun = Curse_updateAll;
				}
				break;
			case 'f':
				force_update = true;
				break;
			case 'c': fun = zero; break;
			case 'v': verbosity++; break;
			case 'd':
				free(dir);
				dir = strdup(optarg);
				break;
			default:
				printf("Unknown option: %c\n", c);
				return -1;
		}
	}

	ConsoleSetVerbosity(verbosity);
	Curse_init(dir);
	free(dir);

	uint8_t free_args = argc-optind;
	if(fun == help){
		help();
	} else if(fun == listItems){
		if(free_args == 0){
			listItems();
		} else {
			printf("Option --list does not accept arguments.\n");
		}
	} else if(fun == Curse_setLocalVersion){
		if(free_args == 2){
			uint32_t version = atoi(argv[optind+1]);
			Curse_setLocalVersion(argv[optind], version);
			printf("Addon %s version was set to %d\n", argv[optind], version);
		} else {
			printf("This option accepts 2 arguments: --set <addon:string> <version:number>\n");
		}
	} else if(fun == DBRemove){
		if(free_args == 1){
			DBRemove(argv[optind]);
			printf("Value %s was removed\n", argv[optind]);
		} else {
			printf("This option accepts only 1 argument: --remove <addon:string>\n");
		}
	} else if(fun == Curse_updateAll){
			if(free_args == 0){
				Curse_updateAll(force_update);
			} else {
				printf("Option --update does not accept additional arguments.\n");
			}
	} else if(fun == Curse_update){
			if(free_args == 0){
				Curse_update(addon_to_update, force_update);
				free(addon_to_update);
			} else {
				printf("Option --update does not accept additional arguments.\n");
			}
	} else if(fun == zero){
		if(free_args == 0){
			zero();
			printf("All addons are now in version 0\n");
		} else {
			printf("Option --clear does not accept arguments.\n");
		}
	}

	Curse_free();

	return 0;
}
