#ifndef	_fun_h_
#define	_fun_h_

#define	funbuff_size 4096
extern char funbuff[funbuff_size];
void vlist_set(va_list *, var *);

// return, call_var, varnumber, varlist
typedef void (*func) (char **, var *, var *, int, var *);

#define	_fun(name) void _func_##name(char **exps, var *ret, var *call, int n, var *vlist)

int _int(var *);
long long _long(var *);
double _float(var *);
#define	_string(vp) (m_leng(vp->mode)==leng_no&&m_type(vp->mode)==type_string)?vp->v.v_string:NULL
#define	_object(vp) (m_leng(vp->mode)==leng_no&&m_type(vp->mode)==type_object)?vp->v.v_object:NULL

// sys
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

// other
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
_fun(ldexp);

// debug
_fun(pv);
_fun(pg);
_fun(po);

#endif

