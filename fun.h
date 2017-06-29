#ifndef	_fun_h_
#define	_fun_h_

#define DL_check 0x6d617468
typedef struct FUNS {
	// string.h
	int (*scmp)(char *, char *);
	char* (*sskip)(char *);
	char* (*sget)(char *);
	int (*scmp_skip)(char **, char *);
	char* (*spget)(char *, int);
	char* (*str_skip)(char *);
	char* (*str_alloc)(char **);
	// var.h
	varmat* (*varmat_alloc)(void);
	void (*varmat_free)(varmat *);
	int (*varmat_ngen)(char *);
	void (*value_free)(var *);
	var* (*var_alloc)(var *, char *, unsigned int, value *);
	var* (*var_ralloc)(var *, char *, unsigned int, value *);
	var* (*var_find)(var *, char *);
	var* (*var_free)(var *, char *);
	void (*var_listfree)(var *);
	var* (*var_listadd)(var *, var *);
	var* (*v_alloc)(varmat *, char *, unsigned int, value *);
	var* (*v_find)(varmat *, char *);
	void (*v_free)(varmat *, char *);
	void (*v_freeall)(varmat *);
	varmat* (*v_list)(varmat *, var *);
	varmat* (*v_add)(varmat *, varmat *);
	var* (*var_poin)(var *, var *);
	char* (*str_type)(unsigned int);
	void (*varmat_print)(varmat *);
	void (*varlist_print)(var *);
	void (*var_print)(var *);
	// fun.h
	void (*vlist_set)(va_list *, var *);
	int (*is_obj)(var *);
	int (*obj_type)(var *, char *, char *);
	int (*is_obj_type)(var *, char *, char *);
	// math.h
	char* (*get_varlist)(char *, int *, var **);
	char* (*run_func)(char *, var **);
	char* (*get_var)(char *, var **, var *);
	char* (*get_float)(char *, double *);
	char* (*cal)(char *, var *);
	int (*math_run)(char *, char *, int , var *);
	// end
	void *null;
} funs;

int _int(var *);
long long _long(var *);
double _float(var *);
#define	_string(vp) (m_leng(vp->mode)==leng_no&&m_type(vp->mode)==type_string)?vp->v.v_string:NULL
#define	_object(vp) (m_leng(vp->mode)==leng_no&&m_type(vp->mode)==type_object)?vp->v.v_object:NULL

// return, call_var, varnumber, varlist
typedef void (*func) (char **, var *, var *, int, var *);

#define	_fun(name) void _func_##name(char **exps, var *ret, var *call, int n, var *vlist)

#ifndef __nosym__

#define	funbuff_size 4096
extern char funbuff[funbuff_size];
void vlist_set(va_list *, var *);
int is_obj(var *);
int obj_type(var *, char *, char *);
int is_obj_type(var *, char *, char *);

// sys
_fun(var);
_fun(jdw);
_fun(jup);
_fun(ldw_test);
_fun(lup_test);
_fun(calldw);
_fun(callup);
_fun(ret);
_fun(if);
_fun(exit);
_fun(echo);
_fun(set);
_fun(unset);
_fun(func);
_fun(load);
_fun(unload);

// other
_fun(void);
_fun(test);
_fun(clear);
_fun(strcpy);
_fun(sprintf);
_fun(strcmp);
_fun(strget);

// math
_fun(int);
_fun(long);
_fun(float);
_fun(cal);
_fun(srand);
_fun(rand);
_fun(sqrt);
_fun(squa);
_fun(pow);
_fun(exp);
_fun(ln);
_fun(lg);
_fun(log);
_fun(sin);
_fun(cos);
_fun(tan);
_fun(asin);
_fun(acos);
_fun(atan);
_fun(sinh);
_fun(cosh);
_fun(tanh);
_fun(asinh);
_fun(acosh);
_fun(atanh);
_fun(ceil);
_fun(floor);
_fun(abs);
_fun(sign);
_fun(mod);
_fun(hypot);
_fun(radian);
_fun(ldexp);

// debug
_fun(pv);
_fun(pg);
_fun(po);

#endif

#endif

