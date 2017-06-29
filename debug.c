#include "math.h"

FILE *_debug_fp;

void _debug_init(char *path)
{
	if (path) _debug_fp=fopen(path,"w");
	else _debug_fp=stdout;
	if (_debug_fp==NULL) _debug_fp=stdout;
}


