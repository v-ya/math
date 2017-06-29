
#include "dl.h"

var *_vl;
funs *fun;
varmat *math_vm;
varmat *glob_vm;
FILE *_debug_fp;

int _int(var *vp)
{
	if (m_leng(vp->mode)==leng_no) switch (m_type(vp->mode))
	{
		case type_int:
			return (int)vp->v.v_int;
		case type_long:
			return (int)vp->v.v_long;
		case type_float:
			return (int)vp->v.v_float;
	}
	return (int)0;
}

long long _long(var *vp)
{
	if (m_leng(vp->mode)==leng_no) switch (m_type(vp->mode))
	{
		case type_int:
			return (long long)vp->v.v_int;
		case type_long:
			return (long long)vp->v.v_long;
		case type_float:
			return (long long)vp->v.v_float;
	}
	return (long long)0;
}

double _float(var *vp)
{
	if (m_leng(vp->mode)==leng_no) switch (m_type(vp->mode))
	{
		case type_int:
			return (double)vp->v.v_int;
		case type_long:
			return (double)vp->v.v_long;
		case type_float:
			return (double)vp->v.v_float;
	}
	return (double)0.0;
}

unsigned int check(unsigned int n)
{
	return n^DL_check;
}

int init(void *vl, void *f, void *vm_m, void *vm_g, void *dfp)
{
	
	_vl=vl;
	fun=f;
	math_vm=vm_m;
	glob_vm=vm_g;
	_debug_fp=dfp;
	return model_init();
}

void uini(void)
{
	model_uini();
	_debug_fp=NULL;
	glob_vm=NULL;
	math_vm=NULL;
	fun=NULL;
	_vl=NULL;
}

int _regfun(var *vl, char *name, func code)
{
	var *vp;
	if (vl)
	{
		if (m_type3(vl->mode)!=type_object) return 2;
		vp=F var_alloc(vl->v.v_object,name,type_void|auth_noset|auth_norev|func_code,NULL);
		if (!vp) return 1;
		vp->v.v_void=code;
	}
	else
	{
		vp=F var_alloc(_vl->v.v_object,name,type_void|auth_noset|auth_norev|func_code,NULL);
		if (!vp) return 1;
		vp->v.v_void=code;
	}
	return 0;
}


