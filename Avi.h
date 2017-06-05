#ifndef	_Avi_h_
#define	_Avi_h_

#define	_fun_avi(name) void _func_avi_##name(char **exps, var *ret, var *call, int n, var *vlist)

_fun_avi(avi_free);
_fun_avi(new);
_fun_avi(close);
_fun_avi(write);

#endif

