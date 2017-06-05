#ifndef	_Wav_h_
#define	_Wav_h_

#define	_fun_wav(name) void _func_wav_##name(char **exps, var *ret, var *call, int n, var *vlist)

_fun_wav(wav_free);
_fun_wav(new);
_fun_wav(load);
_fun_wav(write);
_fun_wav(pset);
_fun_wav(padd);
_fun_wav(pget);
_fun_wav(getmax);
_fun_wav(getcos);
_fun_wav(getsin);
_fun_wav(addcos);
_fun_wav(addsin);
_fun_wav(toleT);
_fun_wav(getT);

#endif

