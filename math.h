#ifndef	_math_h_
#define	_math_h_

#define _debug_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "debug.h"
#include "string.h"
#include "var.h"
#include "fun.h"
#include "fun_ext.h"

#define	VERSION "v1.0.6"

extern varmat *math_vm;
extern varmat *glob_vm;

int glob_init(void);
char* get_varlist(char *, int *, var **);
char* run_func(char *, var **);
char* get_var(char *, var **, var *);
char* get_float(char *, double *);
char* cal(char *, var *);
int math_run(char *, char *, int , var *);

#endif

