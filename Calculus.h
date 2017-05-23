#ifndef	_Calculus_h_
#define	_Calculus_h_

#define	_fun_calc(name) void _func_calc_##name(char **exps, var *ret, var *call, int n, var *vlist)

_fun_calc(calc_free);
_fun_calc(diff);
_fun_calc(diffq);
_fun_calc(diffs);
_fun_calc(inte);
_fun_calc(inteq);
_fun_calc(solv);

#endif

