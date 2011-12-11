/*
 * console.h
 *
 *  Created on: 11-12-2011
 *      Author: arkus
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

typedef enum {
	LOG_LOG,
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFO,
	LOG_DEBUG
} LOG;


extern void ConsoleLog(LOG type, const char* fmt, ...);
extern void ConsoleSetVerbosity(LOG v);

#define DEBUG(...) ConsoleLog(LOG_DEBUG, __VA_ARGS__);
#define INFO(...) ConsoleLog(LOG_INFO, __VA_ARGS__);
#define WARNING(...) ConsoleLog(LOG_WARNING, __VA_ARGS__);
#define ERROR(...) ConsoleLog(LOG_ERROR, __VA_ARGS__);
#define LOG(...) ConsoleLog(LOG_LOG, __VA_ARGS__);

#endif /* CONSOLE_H_ */
