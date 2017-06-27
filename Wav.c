#include "math.h"
#include "Wav.h"
#include "code/wav_h.h"
#include "code/wav.h"

#define	argv(argc) var_find(vlist,spget("_",argc))

static char *pType="Type";
static char *pTypeWav="Wav";

_fun(wav_init)
{
	value v;
	var *vp,*vl;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	vp=v_find(glob_vm,"wav");
	if (vp) return ;
	vp=v_alloc(glob_vm,"wav",type_object|auth_noset|auth_norev,NULL);
	if (!vp) goto err;
	#define	set_fun(f) v.v_void=&_func_wav_##f;vl=var_alloc((var*)vp->v.v_void,#f,type_void|auth_noset|auth_norev|func_code,&v);\
		if (vl) vp->v.v_void=vl;else goto err;
	#define	set_var(n,t,_v) v.v_##t=_v;vl=var_alloc((var*)vp->v.v_void,#n,type_##t|auth_noset,&v);\
		if (vl) vp->v.v_void=vl;else goto err;
	set_var(time,int,Time_max);
	pTime=&(vl->v.v_int);
	set_var(Samp,int,SamplingF);
	vl->mode|=auth_norev;
	set_var(mSamp,int,SamplingFms);
	vl->mode|=auth_norev;
	set_var(mvol,int,0x7fff);
	vl->mode|=auth_norev;
	set_fun(new);
	set_fun(load);
	set_fun(write);
	set_fun(clean);
	set_fun(pset);
	set_fun(padd);
	set_fun(pget);
	set_fun(getmax);
	set_fun(getcos);
	set_fun(getsin);
	set_fun(addcos);
	set_fun(addsin);
	set_fun(toleT);
	set_fun(getT);
	
	vp=v_find(glob_vm,"import");
	if (!vp) goto err;
	set_fun(wav_free);
	#undef	set_fun
	#undef	set_var
	return ;
	err:
	dp("wav_init: wav 初始化失败\n");
	v_free(glob_vm,"wav");
	ret->mode=type_err;
	return ;
}

_fun_wav(wav_free)
{
	var *vp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	v_free(glob_vm,"wav");
	vp=v_find(glob_vm,"import");
	if (vp) vp->v.v_object=var_free(vp->v.v_object,"wav_free");
}

_fun_wav(new)
{
	var *vp,*vl;
	unsigned int mt;
	Wav *wav;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<2) goto err;
	vp=argv(0);
	if (!is_obj(vp)) goto err;
	mt=_int(argv(1));
	wav=Wav_new(mt);
	if (!wav)
	{
		err_alloc:
		if (wav) free(wav);
		err_alloc2:
		dp(".wav.new: 申请 Wav 结构失败\n");
		return ;
	}
	vl=var_ralloc(vp->v.v_object,"_data",type_void|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc;
	vl->v.v_void=wav;
	vl->mode|=free_need;
	vl=var_ralloc(vp->v.v_object,"size",type_int|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc;
	vl->v.v_int=wav->size;
	if (obj_type(vp,pType,pTypeWav)) goto err_alloc2;
	return ;
	err:
	dp(".wav.new: 参数格式错误\n");
}

_fun_wav(load)
{
	var *vp,*vl;
	char *path;
	Wav *wav;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<2) goto err;
	vp=argv(0);
	if (!is_obj(vp)) goto err;
	vl=argv(1);
	path=_string(vl);
	if (!path) goto err;
	wav=Wav_load(path);
	if (!wav)
	{
		err_alloc:
		if (wav) free(wav);
		err_alloc2:
		dp(".wav.load: 加载 %s 失败\n",path);
		return ;
	}
	vl=var_ralloc(vp->v.v_object,"_data",type_void|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc;
	vl->v.v_void=wav;
	vl->mode|=free_need;
	vl=var_ralloc(vp->v.v_object,"size",type_int|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc;
	vl->v.v_int=wav->size;
	if (obj_type(vp,pType,pTypeWav)) goto err_alloc2;
	return ;
	err:
	dp(".wav.load: 参数格式错误\n");
}

Wav* wav_varget(var *vp, char *fun)
{
	if (!is_obj_type(vp,pType,pTypeWav))
	{
		dp("%s: %s 不是 Wav 类型\n",fun,vp->name);
		return NULL;
	}
	vp=var_find(vp->v.v_object,"_data");
	if (!vp)
	{
		dp("%s: _data 数据意外丢失\n",fun);
		return NULL;
	}
	return (Wav*)vp->v.v_void;
}

_fun_wav(write)
{
	var *vp;
	char *path;
	Wav *wav;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<2) goto err;
	vp=argv(1);
	path=_string(vp);
	vp=argv(0);
	wav=wav_varget(vp,".wav.write");
	if (!wav) return ;
	switch(Wav_write(wav,path))
	{
		case 0:
			return ;
		case 1:
			dp(".wav.write: 不能打开 %s\n",path);
			return ;
		case -1:
			dp(".wav.write: Wav 结构出错\n",path);
			return ;
	}
	err:
	dp(".wav.write: 参数格式错误\n");
}

_fun_wav(clean)
{
	var *vp;
	Wav *wav;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<1) goto err;
	vp=argv(0);
	wav=wav_varget(vp,".wav.clean");
	if (!wav) return ;
	if (n==1) Wav_clean(wav,0,wav->size);
	else if (n==2) Wav_clean(wav,_int(argv(1)),wav->size);
	else Wav_clean(wav,_int(argv(1)),_int(argv(2)));
	return ;
	err:
	dp(".wav.clean: 参数格式错误\n");
}

_fun_wav(pset)
{
	var *vp;
	Wav *wav;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<3) goto err;
	vp=argv(0);
	wav=wav_varget(vp,".wav.pset");
	if (!wav) return ;
	Wav_set(wav,_int(argv(1)),_int(argv(2)));
	return ;
	err:
	dp(".wav.pset: 参数格式错误\n");
}

_fun_wav(padd)
{
	var *vp;
	Wav *wav;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<3) goto err;
	vp=argv(0);
	wav=wav_varget(vp,".wav.padd");
	if (!wav) return ;
	Wav_add(wav,_int(argv(1)),_int(argv(2)));
	return ;
	err:
	dp(".wav.padd: 参数格式错误\n");
}

_fun_wav(pget)
{
	var *vp;
	Wav *wav;
	unsigned int t;
	ret->mode=type_int;
	ret->v.v_int=0;
	if (n<2) goto err;
	vp=argv(0);
	wav=wav_varget(vp,".wav.pget");
	if (!wav) return ;
	t=_int(argv(1));
	if (t<wav->size) ret->v.v_int=wav->data[t];
	return ;
	err:
	dp(".wav.pget: 参数格式错误\n");
}

_fun_wav(getmax)
{
	var *vp;
	Wav *wav;
	ret->mode=type_int;
	ret->v.v_int=0;
	if (n<2) goto err;
	vp=argv(0);
	wav=wav_varget(vp,".wav.getmax");
	if (!wav) return ;
	if (n>2) ret->v.v_int=Wav_getmax(wav,_int(argv(1)),_int(argv(2)));
	else ret->v.v_int=Wav_getmax(wav,0,_int(argv(1)));
	return ;
	err:
	dp(".wav.getmax: 参数格式错误\n");
}

_fun_wav(getcos)
{
	var *vp;
	Wav *wav;
	ret->mode=type_float;
	ret->v.v_float=0;
	if (n<3) goto err;
	vp=argv(0);
	wav=wav_varget(vp,".wav.getcos");
	if (!wav) return ;
	if (n>3) ret->v.v_float=Wav_getcos(wav,_int(argv(1)),_int(argv(2)),_int(argv(3)));
	else ret->v.v_float=Wav_getcos(wav,_int(argv(1)),0,_int(argv(2)));
	return ;
	err:
	dp(".wav.getcos: 参数格式错误\n");
}

_fun_wav(getsin)
{
	var *vp;
	Wav *wav;
	ret->mode=type_float;
	ret->v.v_float=0;
	if (n<3) goto err;
	vp=argv(0);
	wav=wav_varget(vp,".wav.getsin");
	if (!wav) return ;
	if (n>3) ret->v.v_float=Wav_getsin(wav,_int(argv(1)),_int(argv(2)),_int(argv(3)));
	else ret->v.v_float=Wav_getsin(wav,_int(argv(1)),0,_int(argv(2)));
	return ;
	err:
	dp(".wav.getsin: 参数格式错误\n");
}

_fun_wav(addcos)
{
	var *vp;
	Wav *wav;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<4) goto err;
	vp=argv(0);
	wav=wav_varget(vp,".wav.addcos");
	if (!wav) return ;
	if (n>4) Wav_addcos(wav,_int(argv(1)),_float(argv(2)),_int(argv(3)),_int(argv(4)));
	else Wav_addcos(wav,_int(argv(1)),_float(argv(2)),0,_int(argv(3)));
	return ;
	err:
	dp(".wav.addcos: 参数格式错误\n");
}

_fun_wav(addsin)
{
	var *vp;
	Wav *wav;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<4) goto err;
	vp=argv(0);
	wav=wav_varget(vp,".wav.addsin");
	if (!wav) return ;
	if (n>4) Wav_addsin(wav,_int(argv(1)),_float(argv(2)),_int(argv(3)),_int(argv(4)));
	else Wav_addsin(wav,_int(argv(1)),_float(argv(2)),0,_int(argv(3)));
	return ;
	err:
	dp(".wav.addsin: 参数格式错误\n");
}

_fun_wav(toleT)
{
	var *vp;
	Wav *wav;
	ret->mode=type_float;
	ret->v.v_float=0;
	if (n<5) goto err;
	vp=argv(0);
	wav=wav_varget(vp,".wav.toleT");
	if (!wav) return ;
	ret->v.v_float=Wav_toleT(wav,_int(argv(1)),_int(argv(2)),_int(argv(3)),_int(argv(4)));
	return ;
	err:
	dp(".wav.toleT: 参数格式错误\n");
}

_fun_wav(getT)
{
	var *vp;
	Wav *wav;
	ret->mode=type_int;
	ret->v.v_int=0;
	if (n<5) goto err;
	vp=argv(0);
	wav=wav_varget(vp,".wav.getT");
	if (!wav) return ;
	ret->v.v_int=Wav_getT(wav,_int(argv(1)),_int(argv(2)),_int(argv(3)),_float(argv(4)));
	return ;
	err:
	dp(".wav.getT: 参数格式错误\n");
}



