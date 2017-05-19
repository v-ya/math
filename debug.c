#include "math.h"

#ifdef	_debug_
	#define	_putc(c) putc(c,_debug_fp)
	static FILE *_debug_fp;
	static char _debug_buffer[128];
	void _dp(const char *s, ...)
	{
		va_list ap;
		va_start(ap,s);
		vfprintf(_debug_fp,s,ap);
		va_end(ap);
	}
	
	void _debug_init(char *path)
	{
		if (path) _debug_fp=fopen(path,"w");
		else _debug_fp=stdout;
		setvbuf(_debug_fp, _debug_buffer, _IOLBF, 128);
	}
#else
	#define	_putc(c) 
	void _debug_init(char *path)
	{
		_debug_fp=NULL;
		return ;
	}
#endif


