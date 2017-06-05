#include "math.h"
#include "Bmp.h"
#include "code/bmp_h.h"
#include "code/bmp.h"

#define	argv(argc) var_find(vlist,spget("_",argc))

static char *pType="Type";
static char *pTypeBmp="Bmp";

static char **pNameX;
static char **pNameY;
static char **pNameT;

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
	// set vname_*
	set_var(vname_x,string,NULL);
	vl->mode|=auth_norev;
	pNameX=&(vl->v.v_string);
	set_var(vname_y,string,NULL);
	vl->mode|=auth_norev;
	pNameY=&(vl->v.v_string);
	set_var(vname_t,string,NULL);
	vl->mode|=auth_norev;
	pNameT=&(vl->v.v_string);
	// set pS*
	set_var(sx,int,0);
	pSx=&(vl->v.v_int);
	set_var(sy,int,0);
	pSy=&(vl->v.v_int);
	set_var(sa,float,0);
	pSa=&(vl->v.v_float);
	// set fun
	set_fun(new);
	set_fun(load);
	set_fun(write);
	set_fun(setxyt);
	set_fun(setcolor);
	set_fun(rgba);
	set_fun(R);
	set_fun(G);
	set_fun(B);
	set_fun(A);
	set_fun(clean);
	set_fun(pget);
	set_fun(pset);
	set_fun(padd);
	set_fun(setalpha);
	set_fun(fset);
	set_fun(fadd);
	set_fun(fill);
	set_fun(ffill);
	set_fun(cset);
	set_fun(cadd);
	set_fun(fcset);
	set_fun(fcadd);
	set_fun(putset);
	set_fun(putadd);
	set_fun(getset);
	set_fun(getadd);
	set_fun(getzoom);
	set_fun(rota);
	set_fun(revw);
	set_fun(revh);
	set_fun(arc_pra);
	set_fun(line);
	set_fun(arc);
	set_fun(next);
	vp=v_find(glob_vm,"import");
	if (!vp) goto err;
	set_fun(bmp_free);
	#undef	set_fun
	#undef	set_var
	return ;
	err:
	dp("bmp_init: bmp 初始化失败\n");
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
	pSx=NULL;
	pSy=NULL;
	pSa=NULL;
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
	if (!vl) goto err_alloc2;
	vl->v.v_int=w;
	vl=var_ralloc(vp->v.v_object,"Height",type_int|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc2;
	vl->v.v_int=h;
	if (obj_type(vp,pType,pTypeBmp)) goto err_alloc2;
	return ;
	err:
	dp(".bmp.new: 参数格式错误\n");
}

_fun_bmp(load)
{
	var *vp,*vl;
	char *path;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<2) goto err;
	vp=argv(0);
	if (!is_obj(vp)) goto err;
	vl=argv(1);
	path=_string(vl);
	if (!path) goto err;
	bmp=Bmp_load(path);
	if (!bmp)
	{
		err_alloc:
		if (bmp) free(bmp);
		err_alloc2:
		dp(".bmp.load: 加载 %s 失败\n",path);
		return ;
	}
	vl=var_ralloc(vp->v.v_object,"_data",type_void|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc;
	vl->v.v_void=bmp;
	vl->mode|=free_need;
	vl=var_ralloc(vp->v.v_object,"Width",type_int|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc2;
	vl->v.v_int=bmp->width;
	vl=var_ralloc(vp->v.v_object,"Height",type_int|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc2;
	vl->v.v_int=bmp->height;
	if (obj_type(vp,pType,pTypeBmp)) goto err_alloc2;
	return ;
	err:
	dp(".bmp.load: 参数格式错误\n");
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

_fun_bmp(setxyt)
{
	var *vl;
	char *x,*y,*t;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n==0)
	{
		free:
		if (pNameX) *pNameX=NULL;
		if (pNameY) *pNameY=NULL;
		if (pNameT) *pNameT=NULL;
		if (pVarX) v_free(math_vm,pVarX->name);
		if (pVarY) v_free(math_vm,pVarY->name);
		if (pVarT) v_free(math_vm,pVarT->name);
		pVarX=NULL;
		pVarY=NULL;
		pVarT=NULL;
		return ;
	}
	else if (n>2)
	{
		while(vlist->l) vlist=vlist->l;
		x=_string(vlist);
		vlist=vlist->r;
		y=_string(vlist);
		vlist=vlist->r;
		t=_string(vlist);
		if (x==NULL||y==NULL||t==NULL)
		{
			dp(".bmp.setxyt: 未能找到合适的变量名称\n");
			return ;
		}
		if ((vl=v_find(math_vm,x))||(vl=v_find(math_vm,y))||(vl=v_find(math_vm,t)))
		{
			dp(".bmp.setxyt: 存在同名变量 %s\n",vl->name);
			return ;
		}
		vl=v_alloc(math_vm,x,type_int|auth_noset,NULL);
		if (vl)
		{
			*pNameX=vl->name;
			if (pVarX) v_free(math_vm,pVarX->name);
			pVarX=vl;
		}
		else goto err_alloc;
		vl=v_alloc(math_vm,y,type_int|auth_noset,NULL);
		if (vl)
		{
			*pNameY=vl->name;
			if (pVarY) v_free(math_vm,pVarY->name);
			pVarY=vl;
		}
		else goto err_alloc;
		vl=v_alloc(math_vm,t,type_float|auth_noset,NULL);
		if (vl)
		{
			*pNameT=vl->name;
			if (pVarT) v_free(math_vm,pVarT->name);
			pVarT=vl;
		}
		else goto err_alloc;
		return ;
		err_alloc:
		dp(".bmp.setxyt: 申请变量失败\n");
		goto free;
	}
	err:
	dp(".bmp.setxyt: 参数格式错误\n");
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

_fun_bmp(rgba)
{
	unsigned int c=0;
	ret->mode=type_int;
	if (n>0) c|=(_int(argv(0))&0xff)<<16;
	if (n>1) c|=(_int(argv(1))&0xff)<<8;
	if (n>2) c|=_int(argv(2))&0xff;
	if (n>3) c|=(_int(argv(3))&0xff)<<24;
	else c|=0xff000000;
	ret->v.v_int=c;
}

_fun_bmp(R)
{
	unsigned int c=0;
	ret->mode=type_int;
	if (n==0)
	{
		c=*pColor;
		ret->v.v_int=(c>>16)&0xff;
	}
	else
	{
		c=_int(argv(0));
		ret->v.v_int=*pColor=((*pColor)&0xff00ffff)+((c&0xff)<<16);
	}
}

_fun_bmp(G)
{
	unsigned int c=0;
	ret->mode=type_int;
	if (n==0)
	{
		c=*pColor;
		ret->v.v_int=(c>>8)&0xff;
	}
	else
	{
		c=_int(argv(0));
		ret->v.v_int=*pColor=((*pColor)&0xffff00ff)+((c&0xff)<<8);
	}
}

_fun_bmp(B)
{
	unsigned int c=0;
	ret->mode=type_int;
	if (n==0)
	{
		c=*pColor;
		ret->v.v_int=c&0xff;
	}
	else
	{
		c=_int(argv(0));
		ret->v.v_int=*pColor=((*pColor)&0xffffff00)+(c&0xff);
	}
}

_fun_bmp(A)
{
	unsigned int c=0;
	ret->mode=type_int;
	if (n==0)
	{
		c=*pColor;
		ret->v.v_int=(c>>24)&0xff;
	}
	else
	{
		c=_int(argv(0));
		ret->v.v_int=*pColor=((*pColor)&0x00ffffff)+((c&0xff)<<24);
	}
}

_fun_bmp(clean)
{
	Bmp *bmp;
	unsigned int color;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<1) goto err;
	if (n==1) color=*pColor;
	else color=_int(argv(1));
	bmp=bmp_varget(argv(0),".bmp.clean");
	if (!bmp) return ;
	Bmp_clean(bmp,color);
	return ;
	err:
	dp(".bmp.clean: 参数格式错误\n");
}

_fun_bmp(pget)
{
	var *vp;
	Bmp *bmp;
	unsigned int x,y;
	ret->mode=type_int;
	ret->v.v_int=0;
	if (n<3) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.pget");
	if (!bmp) return ;
	x=_int(argv(1));
	y=_int(argv(2));
	if (bmp&&x<bmp->width&&y<bmp->height)
	ret->v.v_int=bmp->data[y*bmp->width+x];
	return ;
	err:
	dp(".bmp.pget: 参数格式错误\n");
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

double _test_d(char *fx, char *fy, double a, double b)
{
	var v;
	double r,x,y;
	if (a==b) return 0;
	do
	{
		b=(b+a)/2;
		pVarT->v.v_float=a;
		cal(fx,&v);
		x=v.v.v_float;
		pVarT->v.v_float=a;
		cal(fy,&v);
		y=v.v.v_float;
		pVarT->v.v_float=b;
		cal(fx,&v);
		x-=v.v.v_float;
		pVarT->v.v_float=b;
		cal(fy,&v);
		y-=v.v.v_float;
		r=(x*x)+(y*y);
	}while(r>0.8);
	return (b-a);
}

_fun_bmp(fset)
{
	var *vp,v;
	Bmp *bmp;
	char *fx,*fy;
	double r,e,d;
	int w,h;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<4) goto err;
	if (!pVarT)
	{
		dp(".bmp.fset: 未设置自动变量 t, 请先执行 .bmp.setxyt 函数\n");
		return ;
	}
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.fset");
	if (!bmp) return ;
	vp=argv(1);
	fx=_string(vp);
	vp=argv(2);
	fy=_string(vp);
	if (fx==NULL||fy==NULL) goto err;
	switch(n)
	{
		case 4:
			r=pVarT->v.v_float;
			e=_float(argv(3));
			d=_test_d(fx,fy,r,e);
			break;
		case 5:
			r=_float(argv(3));
			e=_float(argv(4));
			d=_test_d(fx,fy,r,e);
			break;
		default:
			r=_float(argv(3));
			e=_float(argv(4));
			d=_float(argv(5));
			break;
	}
	if (d==0) return ;
	else if (d<0) d=-d;
	if (r>e)
	{
		r+=e;
		e=r-e;
		r=r-e;
	}
	while(r<=e)
	{
		pVarT->v.v_float=r;
		cal(fx,&v);
		w=(int)(v.v.v_float+0.5);
		pVarT->v.v_float=r;
		cal(fy,&v);
		h=(int)(v.v.v_float+0.5);
		Bmp_set(bmp,w,h);
		r+=d;
	}
	return ;
	err:
	dp(".bmp.fset: 传入参数错误\n");
}

_fun_bmp(fadd)
{
	var *vp,v;
	Bmp *bmp;
	char *fx,*fy;
	double r,e,d;
	int w,h;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<4) goto err;
	if (!pVarT)
	{
		dp(".bmp.fadd: 未设置自动变量 t, 请先执行 .bmp.setxyt 函数\n");
		return ;
	}
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.fadd");
	if (!bmp) return ;
	vp=argv(1);
	fx=_string(vp);
	vp=argv(2);
	fy=_string(vp);
	if (fx==NULL||fy==NULL) goto err;
	switch(n)
	{
		case 4:
			r=pVarT->v.v_float;
			e=_float(argv(3));
			d=_test_d(fx,fy,r,e);
			break;
		case 5:
			r=_float(argv(3));
			e=_float(argv(4));
			d=_test_d(fx,fy,r,e);
			break;
		default:
			r=_float(argv(3));
			e=_float(argv(4));
			d=_float(argv(5));
			break;
	}
	if (d==0) return ;
	else if (d<0) d=-d;
	if (r>e)
	{
		r+=e;
		e=r-e;
		r=r-e;
	}
	while(r<=e)
	{
		pVarT->v.v_float=r;
		cal(fx,&v);
		w=(int)(v.v.v_float+0.5);
		pVarT->v.v_float=r;
		cal(fy,&v);
		h=(int)(v.v.v_float+0.5);
		Bmp_add(bmp,w,h);
		r+=d;
	}
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

_fun_bmp(ffill)
{
	var *vp;
	Bmp *bmp;
	char *fc;
	unsigned int cb;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<4) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.ffill");
	if (!bmp) return ;
	vp=argv(1);
	fc=_string(vp);
	if (!fc) goto err;
	cb=*pColor;
	Bmp_ffill(bmp,fc,_int(argv(2)),_int(argv(3)));
	*pColor=cb;
	return ;
	err:
	dp(".bmp.ffill: 参数格式错误\n");
}

_fun_bmp(cset)
{
	var *vp;
	Bmp *bmp;
	char *fc;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n!=2&&n<6) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.cset");
	if (!bmp) return ;
	vp=argv(1);
	fc=_string(vp);
	if (!fc) goto err;
	if (n>5) Bmp_cset(bmp,fc,_int(argv(2)),_int(argv(3)),_int(argv(4)),_int(argv(5)));
	else Bmp_cset(bmp,fc,0,0,bmp->width,bmp->height);
	return ;
	err:
	dp(".bmp.cset: 参数格式错误\n");
}

_fun_bmp(cadd)
{
	var *vp;
	Bmp *bmp;
	char *fc;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n!=2&&n<6) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.cadd");
	if (!bmp) return ;
	vp=argv(1);
	fc=_string(vp);
	if (!fc) goto err;
	if (n>5) Bmp_cadd(bmp,fc,_int(argv(2)),_int(argv(3)),_int(argv(4)),_int(argv(5)));
	else Bmp_cadd(bmp,fc,0,0,bmp->width,bmp->height);
	return ;
	err:
	dp(".bmp.cadd: 参数格式错误\n");
}

_fun_bmp(fcset)
{
	var *vp;
	Bmp *bmp;
	char *fcon,*fc;
	unsigned int cb;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n!=3&&n<7) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.fcset");
	if (!bmp) return ;
	vp=argv(1);
	fcon=_string(vp);
	vp=argv(2);
	fc=_string(vp);
	if (fcon==NULL||fc==NULL) goto err;
	cb=*pColor;
	if (n>6) Bmp_fcset(bmp,fcon,fc,_int(argv(3)),_int(argv(4)),_int(argv(5)),_int(argv(6)));
	else Bmp_fcset(bmp,fcon,fc,0,0,bmp->width,bmp->height);
	*pColor=cb;
	return ;
	err:
	dp(".bmp.fcset: 参数格式错误\n");
}

_fun_bmp(fcadd)
{
	var *vp;
	Bmp *bmp;
	unsigned int cb;
	char *fcon,*fc;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n!=3&&n<7) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.fcadd");
	if (!bmp) return ;
	vp=argv(1);
	fcon=_string(vp);
	vp=argv(2);
	fc=_string(vp);
	if (fcon==NULL||fc==NULL) goto err;
	cb=*pColor;
	if (n>6) Bmp_fcadd(bmp,fcon,fc,_int(argv(3)),_int(argv(4)),_int(argv(5)),_int(argv(6)));
	else Bmp_fcadd(bmp,fcon,fc,0,0,bmp->width,bmp->height);
	*pColor=cb;
	return ;
	err:
	dp(".bmp.fcadd: 参数格式错误\n");
}

_fun_bmp(putset)
{
	var *vp;
	Bmp *bmp,*bmp2;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n!=2&&n<4) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.putset");
	if (!bmp) return ;
	vp=argv(1);
	bmp2=bmp_varget(vp,".bmp.putset");
	if (!bmp2) return ;
	if (n<4) Bmp_put_set(bmp,bmp2,0,0);
	else Bmp_put_set(bmp,bmp2,_int(argv(2)),_int(argv(3)));
	return ;
	err:
	dp(".bmp.putset: 参数格式错误\n");
}

_fun_bmp(putadd)
{
	var *vp;
	Bmp *bmp,*bmp2;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n!=2&&n<4) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.putadd");
	if (!bmp) return ;
	vp=argv(1);
	bmp2=bmp_varget(vp,".bmp.putadd");
	if (!bmp2) return ;
	if (n<4) Bmp_put_add(bmp,bmp2,0,0);
	else Bmp_put_add(bmp,bmp2,_int(argv(2)),_int(argv(3)));
	return ;
	err:
	dp(".bmp.putadd: 参数格式错误\n");
}

_fun_bmp(getset)
{
	var *vp;
	Bmp *bmp,*bmp2;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n!=2&&n<4) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.getset");
	if (!bmp) return ;
	vp=argv(1);
	bmp2=bmp_varget(vp,".bmp.getset");
	if (!bmp2) return ;
	if (n<4) Bmp_get_set(bmp,bmp2,0,0);
	else Bmp_get_set(bmp,bmp2,_int(argv(2)),_int(argv(3)));
	return ;
	err:
	dp(".bmp.getset: 参数格式错误\n");
}

_fun_bmp(getadd)
{
	var *vp;
	Bmp *bmp,*bmp2;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n!=2&&n<4) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.getadd");
	if (!bmp) return ;
	vp=argv(1);
	bmp2=bmp_varget(vp,".bmp.getadd");
	if (!bmp2) return ;
	if (n<4) Bmp_get_add(bmp,bmp2,0,0);
	else Bmp_get_add(bmp,bmp2,_int(argv(2)),_int(argv(3)));
	return ;
	err:
	dp(".bmp.getadd: 参数格式错误\n");
}

_fun_bmp(getzoom)
{
	var *vp;
	Bmp *bmp,*bmp2;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<2) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.getzoom");
	if (!bmp) return ;
	vp=argv(1);
	bmp2=bmp_varget(vp,".bmp.getzoom");
	if (!bmp2) return ;
	Bmp_get_zoom(bmp,bmp2);
	return ;
	err:
	dp(".bmp.getzoom: 参数格式错误\n");
}

_fun_bmp(rota)
{
	var *vp;
	Bmp *bmp,*bmp2;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<5) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.rota");
	if (!bmp) return ;
	vp=argv(1);
	bmp2=bmp_varget(vp,".bmp.rota");
	if (!bmp2) return ;
	Bmp_rota(bmp,bmp2,_float(argv(2)),_float(argv(3)),_float(argv(4)));
	return ;
	err:
	dp(".bmp.rota: 参数格式错误\n");
}

_fun_bmp(revw)
{
	var *vp;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<1) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.revw");
	if (!bmp) return ;
	Bmp_revw(bmp);
	return ;
	err:
	dp(".bmp.revw: 参数格式错误\n");
}

_fun_bmp(revh)
{
	var *vp;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<1) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.revh");
	if (!bmp) return ;
	Bmp_revh(bmp);
	return ;
	err:
	dp(".bmp.revh: 参数格式错误\n");
}

_fun_bmp(arc_pra)
{
	var *vp;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<6) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.arc_pra");
	if (!bmp) return ;
	Bmp_arc_pra(bmp,_float(argv(1)),_float(argv(2)),_float(argv(3)),_float(argv(4)),_float(argv(5)));
	return ;
	err:
	dp(".bmp.arc_pra: 参数格式错误\n");
}

_fun_bmp(line)
{
	var *vp;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<5) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.line");
	if (!bmp) return ;
	Bmp_line(bmp,_float(argv(1)),_float(argv(2)),_float(argv(3)),_float(argv(4)));
	return ;
	err:
	dp(".bmp.line: 参数格式错误\n");
}

_fun_bmp(arc)
{
	var *vp;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<7) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.arc");
	if (!bmp) return ;
	Bmp_arc_3p(bmp,_int(argv(1)),_int(argv(2)),_int(argv(3)),_int(argv(4)),_int(argv(5)),_int(argv(6)));
	return ;
	err:
	dp(".bmp.arc: 参数格式错误\n");
}

_fun_bmp(next)
{
	var *vp;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<3) goto err;
	vp=argv(0);
	bmp=bmp_varget(vp,".bmp.next");
	if (!bmp) return ;
	Bmp_arc_next(bmp,_int(argv(1)),_int(argv(2)));
	return ;
	err:
	dp(".bmp.next: 参数格式错误\n");
}


