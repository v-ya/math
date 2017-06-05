#ifndef	_Bmp_h_
#define	_Bmp_h_

#define	_fun_bmp(name) void _func_bmp_##name(char **exps, var *ret, var *call, int n, var *vlist)

_fun_bmp(bmp_free);
_fun_bmp(new);
_fun_bmp(load);
_fun_bmp(write);
_fun_bmp(setxyt);
_fun_bmp(setcolor);
_fun_bmp(rgba);
_fun_bmp(R);
_fun_bmp(G);
_fun_bmp(B);
_fun_bmp(A);
_fun_bmp(clean);
_fun_bmp(pget);
_fun_bmp(pset);
_fun_bmp(padd);
_fun_bmp(setalpha);
_fun_bmp(fset);
_fun_bmp(fadd);
_fun_bmp(fill);
_fun_bmp(ffill);
_fun_bmp(cset);
_fun_bmp(cadd);
_fun_bmp(fcset);
_fun_bmp(fcadd);
_fun_bmp(putset);
_fun_bmp(putadd);
_fun_bmp(getset);
_fun_bmp(getadd);
_fun_bmp(getzoom);
_fun_bmp(rota);
_fun_bmp(revw);
_fun_bmp(revh);
_fun_bmp(arc_pra);
_fun_bmp(line);
_fun_bmp(arc);
_fun_bmp(next);

#endif

