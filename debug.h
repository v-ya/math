#ifndef	_debug_h_
#define	_debug_h_

#ifdef	_debug_
	#include <stdio.h>
	#define	dp(...) _dp(__VA_ARGS__)
	void _dp(const char *, ...);
#else
	#define dp(...) 
#endif

void _debug_init(char *path);
char* dprint(char *);

#endif

