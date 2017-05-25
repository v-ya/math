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
	set_fun(new);
	set_fun(write);
	set_fun(pset);
	
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


