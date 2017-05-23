#include "math.h"
#include "Bmp.h"
#include "code/bmp.h"

#define	argv(argc) var_find(vlist,spget("_",argc))

static char *pType="Type";
static char *pTypeBmp="Bmp";

_fun(bmp_init)
{
	value v;
	var *vp,*vl;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	vp=v_find(glob_vm,"bmp");
	if (vp) return ;
	vp=v_alloc(glob_vm,"bmp",type_object|auth_noset|auth_norev,NULL);
	if (!vp) goto err;
	#define	set_fun(f) v.v_void=&_func_bmp_##f;vl=var_alloc((var*)vp->v.v_void,#f,type_void|auth_noset|auth_norev|func_code,&v);\
		if (vl) vp->v.v_void=vl;else goto err;
	#define	set_var(n,t,_v) v.v_##t=_v;vl=var_alloc((var*)vp->v.v_void,#n,type_##t|auth_noset,&v);\
		if (vl) vp->v.v_void=vl;else goto err;
	set_var(width,int,Width_max);
	pWidth=&(vl->v.v_int);
	set_var(height,int,Height_max);
	pHeight=&(vl->v.v_int);
	set_var(color,int,0xff000000);
	pColor=&(vl->v.v_int);
	set_fun(new);
	set_fun(write);
	set_fun(setcolor);
	set_fun(pset);
	set_fun(padd);
	set_fun(setalpha);
	set_fun(fset);
	set_fun(fadd);
	set_fun(fill);
	set_fun(blockset);
	set_fun(blockadd);
	vp=v_find(glob_vm,"import");
	if (!vp) goto err;
	set_fun(bmp_free);
	#undef	set_fun
	#undef	set_var
	return ;
	err:
	dp("bmp_init: calc 初始化失败\n");
	v_free(glob_vm,"bmp");
	ret->mode=type_err;
	return ;
}

_fun_bmp(bmp_free)
{
	var *vp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	v_free(glob_vm,"bmp");
	vp=v_find(glob_vm,"import");
	if (vp) vp->v.v_object=var_free(vp->v.v_object,"bmp_free");
	pWidth=NULL;
	pHeight=NULL;
	pColor=NULL;
}

_fun_bmp(new)
{
	var *vp,*vl;
	int w,h;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<3) goto err;
	vp=argv(0);
	if (!is_obj(vp)) goto err;
	w=_int(argv(1));
	h=_int(argv(2));
	bmp=Bmp_new(w,h);
	if (!bmp)
	{
		err_alloc:
		if (bmp) free(bmp);
		err_alloc2:
		dp(".bmp.new: 申请 Bmp 结构失败\n");
		return ;
	}
	vl=var_ralloc(vp->v.v_object,"_data",type_void|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc;
	vl->v.v_void=bmp;
	vl->mode|=free_need;
	vl=var_ralloc(vp->v.v_object,"Width",type_int|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc;
	vl->v.v_int=w;
	vl=var_ralloc(vp->v.v_object,"Height",type_int|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc;
	vl->v.v_int=h;
	if (obj_type(vp,pType,pTypeBmp)) goto err_alloc2;
	return ;
	err:
	dp(".bmp.new: 参数格式错误\n");
}

Bmp* bmp_varget(var *vp, char *fun)
{
	if (!is_obj_type(vp,pType,pTypeBmp))
	{
		dp("%s: %s 不是 Bmp 类型\n",fun,vp->name);
		return NULL;
	}
	vp=var_find(vp->v.v_object,"_data");
	if (!vp)
	{
		dp("%s: _data 数据意外丢失\n",fun);
		return NULL;
	}
	return (Bmp*)vp->v.v_void;
}

_fun_bmp(write)
{
	var *vp;
	char *path;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<2) goto err;
	vp=argv(1);
	path=_string(vp);
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.write");
	if (!bmp) return ;
	switch(Bmp_write(bmp,path))
	{
		case 0:
			return ;
		case 1:
			dp(".bmp.write: 不能打开 %s\n",path);
			return ;
		case -1:
			dp(".bmp.write: Bmp 结构出错\n",path);
			return ;
	}
	err:
	dp(".bmp.write: 参数格式错误\n");
}

_fun_bmp(setcolor)
{
	unsigned int c=0;
	ret->mode=type_int;
	if (n>0) c|=(_int(argv(0))&0xff)<<16;
	if (n>1) c|=(_int(argv(1))&0xff)<<8;
	if (n>2) c|=_int(argv(2))&0xff;
	if (n>3) c|=(_int(argv(3))&0xff)<<24;
	else c|=0xff000000;
	*pColor=c;
	ret->v.v_int=c;
}

_fun_bmp(pset)
{
	var *vp;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<3) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.pset");
	if (!bmp) return ;
	Bmp_set(bmp,_int(argv(1)),_int(argv(2)));
	return ;
	err:
	dp(".bmp.pset: 参数格式错误\n");
}

_fun_bmp(padd)
{
	var *vp;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<3) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.padd");
	if (!bmp) return ;
	Bmp_add(bmp,_int(argv(1)),_int(argv(2)));
	return ;
	err:
	dp(".bmp.padd: 参数格式错误\n");
}

_fun_bmp(setalpha)
{
	var *vp;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<2) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.setalpha");
	if (!bmp) return ;
	Bmp_set_alpha(bmp,_int(argv(1)));
	return ;
	err:
	dp(".bmp.setalpha: 参数格式错误\n");
}

_fun_bmp(fset)
{
	var *vp,v;
	Bmp *bmp;
	char *fx,*fy,*name;
	double xb,r,e,d;
	int w,h;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<5) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.fset");
	if (!bmp) return ;
	vp=argv(1);
	fx=_string(vp);
	vp=argv(2);
	fy=_string(vp);
	vp=argv(3);
	name=_string(vp);
	if (fx==NULL||fy==NULL||name==NULL) goto err;
	get_var(name,&vp,&v);
	if (!vp) goto err;
	if (m_type3(vp->mode)!=type_float) goto err;
	xb=vp->v.v_float;
	switch(n)
	{
		case 5:
			r=xb;
			e=_float(argv(4));
			d=1;
			break;
		case 6:
			r=xb;
			e=_float(argv(4));
			d=_float(argv(5));
			break;
		default:
			r=_float(argv(4));
			e=_float(argv(5));
			d=_float(argv(6));
			break;
	}
	if (d==0) d=1;
	else if (d<0) d=-d;
	if (r>e)
	{
		r+=e;
		e=r-e;
		r=r-e;
	}
	while(r<=e)
	{
		vp->v.v_float=r;
		cal(fx,&v);
		w=(int)(v.v.v_float+0.5);
		vp->v.v_float=r;
		cal(fy,&v);
		h=(int)(v.v.v_float+0.5);
		Bmp_set(bmp,w,h);
		r+=d;
	}
	vp->v.v_float=xb;
	return ;
	err:
	dp(".bmp.fset: 传入参数错误\n");
}

_fun_bmp(fadd)
{
	var *vp,v;
	Bmp *bmp;
	char *fx,*fy,*name;
	double xb,r,e,d;
	int w,h;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<5) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.fadd");
	if (!bmp) return ;
	vp=argv(1);
	fx=_string(vp);
	vp=argv(2);
	fy=_string(vp);
	vp=argv(3);
	name=_string(vp);
	if (fx==NULL||fy==NULL||name==NULL) goto err;
	get_var(name,&vp,&v);
	if (!vp) goto err;
	if (m_type3(vp->mode)!=type_float) goto err;
	xb=vp->v.v_float;
	switch(n)
	{
		case 5:
			r=xb;
			e=_float(argv(4));
			d=1;
			break;
		case 6:
			r=xb;
			e=_float(argv(4));
			d=_float(argv(5));
			break;
		default:
			r=_float(argv(4));
			e=_float(argv(5));
			d=_float(argv(6));
			break;
	}
	if (d==0) d=1;
	else if (d<0) d=-d;
	if (r>e)
	{
		r+=e;
		e=r-e;
		r=r-e;
	}
	while(r<=e)
	{
		vp->v.v_float=r;
		cal(fx,&v);
		w=(int)(v.v.v_float+0.5);
		vp->v.v_float=r;
		cal(fy,&v);
		h=(int)(v.v.v_float+0.5);
		Bmp_set(bmp,w,h);
		r+=d;
	}
	vp->v.v_float=xb;
	return ;
	err:
	dp(".bmp.fadd: 传入参数错误\n");
}

_fun_bmp(fill)
{
	var *vp;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<3) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.fill");
	if (!bmp) return ;
	Bmp_fill(bmp,_int(argv(1)),_int(argv(2)));
	return ;
	err:
	dp(".bmp.fill: 参数格式错误\n");
}

_fun_bmp(blockset)
{
	var *vp;
	Bmp *bmp,*bmp2;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<4) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.blockset");
	if (!bmp) return ;
	vp=argv(1);
	bmp2=bmp_varget(vp,".bmp.blockset");
	if (!bmp2) return ;
	Bmp_block_set(bmp,bmp2,_int(argv(2)),_int(argv(3)));
	return ;
	err:
	dp(".bmp.blockset: 参数格式错误\n");
}

_fun_bmp(blockadd)
{
	var *vp;
	Bmp *bmp,*bmp2;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<4) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.blockadd");
	if (!bmp) return ;
	vp=argv(1);
	bmp2=bmp_varget(vp,".bmp.blockadd");
	if (!bmp2) return ;
	Bmp_block_add(bmp,bmp2,_int(argv(2)),_int(argv(3)));
	return ;
	err:
	dp(".bmp.blockadd: 参数格式错误\n");
}


