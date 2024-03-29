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

#ifndef _My_COMMON
	#define _My_COMMON

	#include <stdint.h>
	typedef uint8_t bool;
	#define false 0
	#define true  1

	#ifndef NULL
		#define NULL 0L
	#endif

	#ifdef X86_64
		#define LLU "%llu"
	#else
		#define LLU "%lu"
	#endif

#endif
