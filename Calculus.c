#include "math.h"
#include "Calculus.h"
#include "code/calculus_h.h"
#include "code/calculus.h"

#define	argv(argc) var_find(vlist,spget("_",argc))

_fun(calc_init)
{
	value v;
	var *vp,*vl;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	vp=v_find(glob_vm,"calc");
	if (vp) return ;
	vp=v_alloc(glob_vm,"calc",type_object|auth_noset|auth_norev,NULL);
	if (!vp) goto err;
	#define	set_fun(f) v.v_void=&_func_calc_##f;vl=var_alloc((var*)vp->v.v_void,#f,type_void|auth_noset|auth_norev|func_code,&v);\
		if (vl) vp->v.v_void=vl;else goto err;
	#define	set_var(n,t,_v) v.v_##t=_v;vl=var_alloc((var*)vp->v.v_void,#n,type_##t|auth_noset,&v);\
		if (vl) vp->v.v_void=vl;else goto err;
	set_var(acc,float,ldexp(1,-26));
	_acc=&(vl->v.v_float);
	set_var(inn,float,ldexp(1,-10));
	_inn=&(vl->v.v_float);
	set_var(solv_in,int,100);
	_solv_in=&(vl->v.v_int);
	set_var(solv_ac,float,0.0);
	_solv_ac=&(vl->v.v_float);
	set_fun(diff);
	set_fun(diffq);
	set_fun(diffs);
	set_fun(inte);
	set_fun(inteq);
	set_fun(solv);
	
	vp=v_find(glob_vm,"import");
	if (!vp) goto err;
	set_fun(calc_free);
	#undef	set_fun
	#undef	set_var
	return ;
	err:
	dp("calc_init: calc 初始化失败\n");
	v_free(glob_vm,"calc");
	ret->mode=type_err;
	return ;
}

_fun_calc(calc_free)
{
	var *vp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	v_free(glob_vm,"calc");
	vp=v_find(glob_vm,"import");
	if (vp) vp->v.v_object=var_free(vp->v.v_object,"calc_free");
	_acc=NULL;
	_inn=NULL;
	_solv_in=NULL;
	_solv_ac=NULL;
}

_fun_calc(diff)
{
	var *vp,v;
	char *exp,*name;
	double x;
	ret->mode=type_float;
	ret->v.v_float=0;
	if (n>1)
	{
		vp=argv(0);
		exp=_string(vp);
		vp=argv(1);
		name=_string(vp);
		if (exp==NULL||name==NULL) goto err;
		get_var(name,&vp,&v);
		if (!vp) goto err;
		if (m_type3(vp->mode)!=type_float) goto err;
		x=vp->v.v_float;
		if (n>2) vp->v.v_float=_float(argv(2));
		ret->v.v_float=_calc_diff(exp,vp);
		vp->v.v_float=x;
		return ;
	}
	err:
	dp(".calc.diff: 传入参数错误\n");
}

_fun_calc(diffq)
{
	var *vp,v;
	char *exp,*name;
	double x;
	ret->mode=type_float;
	ret->v.v_float=0;
	if (n>1)
	{
		vp=argv(0);
		exp=_string(vp);
		vp=argv(1);
		name=_string(vp);
		if (exp==NULL||name==NULL) goto err;
		get_var(name,&vp,&v);
		if (!vp) goto err;
		if (m_type3(vp->mode)!=type_float) goto err;
		x=vp->v.v_float;
		if (n>2) vp->v.v_float=_float(argv(2));
		ret->v.v_float=_calc_diffq(exp,vp);
		vp->v.v_float=x;
		return ;
	}
	err:
	dp(".calc.diffq: 传入参数错误\n");
}

_fun_calc(diffs)
{
	var *vp,v;
	char *name,*src;
	int err=0;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n>2)
	{
		vp=argv(0);
		name=_string(vp);
		vp=argv(1);
		src=_string(vp);
		vp=argv(2);
		x=_string(vp);
		if (name) get_var(name,&vp,&v);
		else goto err;
		if (m_type3(vp->mode)!=type_string)
		{
			dp(".calc.diffs: 不能找到 string 类型的 %s 变量\n",name);
			goto end;
		}
		else if (m_auth_rev(vp->mode))
		{
			dp(".calc.diffs: 无权修改变量 %s\n",name);
			goto end;
		}
		if (x==NULL)
		{
			dp(".calc.diffs: 未指定被微分变量\n");
			goto end;
		}
		value_free(vp);
		if (src) vp->v.v_string=_calc_diffs(src,&err);
		if (vp->v.v_string&&err>=0) vp->mode|=free_need;
		else
		{
			dp(".calc.diffs: 微分运算过程出错\n");
			if (vp->v.v_string) free(vp->v.v_string);
			goto end;
		}
		end:
		x=NULL;
		return ;
	}
	err:
	dp(".calc.diffs: 传递非法参数\n");
	#undef	_is_0
	#undef	_is_1
}

_fun_calc(inte)
{
	var *vp,v;
	char *exp,*name;
	double x;
	ret->mode=type_float;
	ret->v.v_float=0;
	if (n>2)
	{
		vp=argv(0);
		exp=_string(vp);
		vp=argv(1);
		name=_string(vp);
		if (exp==NULL||name==NULL) goto err;
		get_var(name,&vp,&v);
		if (!vp) goto err;
		if (m_type3(vp->mode)!=type_float) goto err;
		x=vp->v.v_float;
		if (n>3)
		{
			vp->v.v_float=_float(argv(2));
			ret->v.v_float=_calc_inte(exp,vp,_float(argv(3)));
		}
		else ret->v.v_float=_calc_inte(exp,vp,_float(argv(2)));
		vp->v.v_float=x;
		return ;
	}
	err:
	dp(".calc.inte: 传入参数错误\n");
}


_fun_calc(inteq)
{
	var *vp,v;
	char *exp,*name;
	double x;
	ret->mode=type_float;
	ret->v.v_float=0;
	if (n>2)
	{
		vp=argv(0);
		exp=_string(vp);
		vp=argv(1);
		name=_string(vp);
		if (exp==NULL||name==NULL) goto err;
		get_var(name,&vp,&v);
		if (!vp) goto err;
		if (m_type3(vp->mode)!=type_float) goto err;
		x=vp->v.v_float;
		if (n>3)
		{
			vp->v.v_float=_float(argv(2));
			ret->v.v_float=_calc_inteq(exp,vp,_float(argv(3)));
		}
		else ret->v.v_float=_calc_inteq(exp,vp,_float(argv(2)));
		vp->v.v_float=x;
		return ;
	}
	err:
	dp(".calc.inteq: 传入参数错误\n");
}

_fun_calc(solv)
{
	var *vp,v;
	char *exp,*name;
	double x;
	ret->mode=type_float;
	ret->v.v_float=0;
	if (n>1)
	{
		vp=argv(0);
		exp=_string(vp);
		vp=argv(1);
		name=_string(vp);
		if (exp==NULL||name==NULL) goto err;
		get_var(name,&vp,&v);
		if (!vp) goto err;
		if (m_type3(vp->mode)!=type_float) goto err;
		x=vp->v.v_float;
		if (n>2) vp->v.v_float=_float(argv(2));
		ret->v.v_float=_calc_solv(exp,vp);
		vp->v.v_float=x;
		return ;
	}
	err:
	dp(".calc.solv: 传入参数错误\n");
}



