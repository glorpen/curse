/*
 * console.c
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
