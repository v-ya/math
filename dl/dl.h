#ifndef _dl_h_
#define _dl_h_
#include <stdio.h>
#include <stdlib.h>

#define __nosym__
#include "../debug.h"
#include "../var.h"
#include "../fun.h"

extern var *_vl;
extern funs *fun;
extern varmat *math_vm;
extern varmat *glob_vm;
extern FILE *_debug_fp;

#define F fun->

int model_init(void);
void model_uini(void);
int _regfun(var *, char *, func);
#define regfun(n,c) _regfun(NULL,n,c)


#endif

