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

#define DEBUG(...) printf(__VA_ARGS__)

#endif
