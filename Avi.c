#include "math.h"
#include "Avi.h"
#include "code/bmp_h.h"
#include "code/avi_h.h"
#include "code/avi.h"

#define	argv(argc) var_find(vlist,spget("_",argc))

static char *pType="Type";
static char *pTypeBmp="Bmp";
static char *pTypeAvi="Avi";

_fun(avi_init)
{
	value v;
	var *vp,*vl;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	vp=v_find(glob_vm,"avi");
	if (vp) return ;
	vp=v_alloc(glob_vm,"avi",type_object|auth_noset|auth_norev,NULL);
	if (!vp) goto err;
	#define	set_fun(f) v.v_void=&_func_avi_##f;vl=var_alloc((var*)vp->v.v_void,#f,type_void|auth_noset|auth_norev|func_code,&v);\
		if (vl) vp->v.v_void=vl;else goto err;
	#define	set_var(n,t,_v) v.v_##t=_v;vl=var_alloc((var*)vp->v.v_void,#n,type_##t|auth_noset,&v);\
		if (vl) vp->v.v_void=vl;else goto err;
	set_var(width,int,Width_max);
	pWidth=&(vl->v.v_int);
	set_var(height,int,Height_max);
	pHeight=&(vl->v.v_int);
	set_var(fps,int,FPS_max);
	pFps=&(vl->v.v_int);
	set_fun(new);
	set_fun(close);
	set_fun(write);
	vp=v_find(glob_vm,"import");
	if (!vp) goto err;
	set_fun(avi_free);
	#undef	set_fun
	#undef	set_var
	return ;
	err:
	dp("avi_init: avi 初始化失败\n");
	v_free(glob_vm,"avi");
	ret->mode=type_err;
	return ;
}

_fun_avi(avi_free)
{
	var *vp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	v_free(glob_vm,"avi");
	vp=v_find(glob_vm,"import");
	if (vp) vp->v.v_object=var_free(vp->v.v_object,"avi_free");
	pWidth=NULL;
	pHeight=NULL;
	pFps=NULL;
}

_fun_avi(new)
{
	var *vp,*vl;
	char *path;
	int w,h;
	Avi *avi;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<4) goto err;
	vp=argv(1);
	path=_string(vp);
	if (!path) goto err;
	vp=argv(0);
	if (!is_obj(vp)) goto err;
	w=_int(argv(2));
	h=_int(argv(3));
	if (n>4) avi=Avi_new(path,w,h,_int(argv(4)));
	else avi=Avi_new(path,w,h,25);
	if (!avi)
	{
		err_alloc:
		if (avi) free(avi);
		err_alloc2:
		dp(".avi.new: 申请 Avi 结构失败\n");
		return ;
	}
	vl=var_ralloc(vp->v.v_object,"_data",type_void|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc;
	vl->v.v_void=avi;
	vl->mode|=free_need;
	vl=var_ralloc(vp->v.v_object,"Width",type_int|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc2;
	vl->v.v_int=w;
	vl=var_ralloc(vp->v.v_object,"Height",type_int|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc2;
	vl->v.v_int=h;
	vl=var_ralloc(vp->v.v_object,"Fps",type_int|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc2;
	vl->v.v_int=avi->fps;
	vl=var_ralloc(vp->v.v_object,"TotalFs",type_int|auth_noset|auth_norev,NULL);
	if (!vl) goto err_alloc2;
	vl->v.v_int=avi->totalFs;
	if (obj_type(vp,pType,pTypeAvi)) goto err_alloc2;
	return ;
	err:
	dp(".avi.new: 参数格式错误\n");
}

Avi* avi_varget(var *vp, char *fun)
{
	if (!is_obj_type(vp,pType,pTypeAvi))
	{
		dp("%s: %s 不是 Avi 类型\n",fun,vp->name);
		return NULL;
	}
	vp=var_find(vp->v.v_object,"_data");
	if (!vp)
	{
		dp("%s: _data 数据意外丢失\n",fun);
		return NULL;
	}
	return (Avi*)vp->v.v_void;
}

_fun_avi(close)
{
	var *vp;
	Avi *avi;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<1) return ;
	vp=argv(0);
	avi=avi_varget(vp,".avi.close");
	if (!avi) return ;
	if (avi->fp)
	{
		fseek(avi->fp,0,SEEK_SET);
		fwrite(&(avi->hi),sizeof(Avi_hi),1,avi->fp);
		fclose(avi->fp);
		avi->fp=NULL;
	}
	vp=var_find(vp->v.v_object,pType);
	if (vp) vp->v.v_string=NULL;
}

_fun_avi(write)
{
	var *vp;
	Avi *avi;
	Bmp *bmp;
	unsigned int nf;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<2) goto err;
	vp=argv(0);
	avi=avi_varget(vp,".avi.write");
	if (!avi) return ;
	vp=argv(1);
	bmp=bmp_varget(vp,".avi.write");
	if (!bmp) return ;
	if (n>2) nf=_int(argv(2));
	else nf=avi->totalFs;
	switch(Avi_write(avi,bmp,nf))
	{
		case 0:
			vp=argv(0);
			vp=var_find(vp->v.v_object,"TotalFs");
			if (vp) vp->v.v_int=avi->totalFs;
			return ;
		case 1:
			dp(".avi.write: Avi 与 Bmp 尺寸不符\n");
			return ;
		case 2:
			dp(".avi.write: 指定帧数超限 %d>%d\n",nf,avi->totalFs);
			return ;
		case 3:
			dp(".avi.write: 文件过大，超过 4GB 限制\n",nf,avi->totalFs);
			return ;
		case -1:
			dp(".avi.write: Avi 结构出错\n");
			return ;
	}
	err:
	dp(".avi.write: 参数格式错误\n");
}
