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
 *  Created on: 11-12-2011
 *      Author: arkus
 */

#include "common.h"
#include "console.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static int tab_count = 0;
static LOG verbosity = 0;

void ConsoleLog(LOG type, const char* fmt, ...){
	va_list v;
	char format[256];

	if(verbosity<type) return;

	sprintf(format, "%s\n", fmt);

	va_start(v, fmt);
	vprintf(format, v);
	va_end(v);

}

void ConsoleSetTab(int count){
	tab_count = count;
}

void ConsoleSetVerbosity(LOG v){
	verbosity = v;
}
