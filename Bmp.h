#ifndef	_Bmp_h_
#define	_Bmp_h_

#define	_fun_bmp(name) void _func_bmp_##name(char **exps, var *ret, var *call, int n, var *vlist)

_fun_bmp(bmp_free);
_fun_bmp(new);
_fun_bmp(write);
_fun_bmp(setcolor);
_fun_bmp(pset);
_fun_bmp(padd);
_fun_bmp(setalpha);
_fun_bmp(fset);
_fun_bmp(fadd);
_fun_bmp(fill);
_fun_bmp(blockset);
_fun_bmp(blockadd);

#endif

