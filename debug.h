#ifndef	_debug_h_
#define	_debug_h_

#define	dp(...) fprintf(_debug_fp,__VA_ARGS__)

#ifndef __nosym__

extern FILE *_debug_fp;
void _debug_init(char *path);

#endif

#endif

