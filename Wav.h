#ifndef	_Wav_h_
#define	_Wav_h_

#define	_fun_wav(name) void _func_wav_##name(char **exps, var *ret, var *call, int n, var *vlist)

_fun_wav(wav_free);
_fun_wav(new);
_fun_wav(write);
_fun_wav(pset);

#endif

