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
