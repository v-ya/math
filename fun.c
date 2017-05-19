#include "math.h"

#define	argv(argc) var_find(vlist,spget("_",argc))

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

_fun(jdw)
{
	char *s=*exps,*lab=NULL;
	var *vp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n>0)
	{
		vp=argv(0);
		if (m_leng(vp->mode)==leng_no&&m_type(vp->mode)==type_string)
		lab=vp->v.v_string;
	}
	if (!lab) goto end;
	while(*s)
	{
		while(*s&&*s!='\n') s++;
		if (*s) s++;
		if (*s=='#'&&scmp(s+1,lab)) goto end;
	}
	dp(".jdw 不能找到标签 %s\n",lab);
	return ;
	end:
	*exps=s;
}

_fun(jup)
{
	char *s=*exps,*lab=NULL,*start;
	var *vp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	vp=v_find(math_vm,"_start");
	if (!vp)
	{
		dp(".jup: 不能找到预设变量 _start\n");
		goto end;
	}
	start=(char*)vp->v.v_void;
	if (n>0)
	{
		vp=argv(0);
		if (m_leng(vp->mode)==leng_no&&m_type(vp->mode)==type_string)
		lab=vp->v.v_string;
	}
	if (!lab) goto end;
	while(s>start)
	{
		while(s>start&&*s!='\n') s--;
		if (*s=='\n') s++;
		if (*s=='#'&&scmp(s+1,lab)) goto end;
		if (s>start&&*(s-1)=='\n') s--;
		if (s>start&&*s=='\n') s--;
	}
	dp(".jup 不能找到标签 %s\n",lab);
	return ;
	end:
	*exps=s;
}

_fun(calldw)
{
	char *s=*exps,*lab;
	var *vp;
	int *sp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	vp=v_find(math_vm,"_sp");
	if (!vp)
	{
		dp(".calldw: 不能找到预设变量 _sp\n");
		return ;
	}
	sp=&(vp->v.v_int);
	lab=spget("_s_",*sp);
	while(*s&&*s!=';') s++;
	vp=v_alloc(math_vm,lab,m_gen(type_void,auth_noset|auth_norev,leng_no),(value*)&s);
	if (vp) (*sp)++;
	else
	{
		dp(".calldw: 不能设置变量 %s\n",lab);
		return ;
	}
	s=*exps;
	lab=NULL;
	if (n>0)
	{
		vp=argv(0);
		if (m_leng(vp->mode)==leng_no&&m_type(vp->mode)==type_string)
		lab=vp->v.v_string;
	}
	if (!lab) goto end;
	while(*s)
	{
		while(*s&&*s!='\n') s++;
		if (*s) s++;
		if (*s=='#'&&scmp(s+1,lab)) goto end;
	}
	end:
	*exps=s;
}

_fun(callup)
{
	char *s=*exps,*lab,*start;
	var *vp;
	int *sp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	vp=v_find(math_vm,"_sp");
	if (!vp)
	{
		dp(".callup: 不能找到预设变量 _sp\n");
		return ;
	}
	sp=&(vp->v.v_int);
	lab=spget("_s_",*sp);
	while(*s&&*s!=';') s++;
	vp=v_alloc(math_vm,lab,m_gen(type_void,auth_noset|auth_norev,leng_no),(value*)&s);
	if (vp) (*sp)++;
	else
	{
		dp(".callup: 不能设置变量 %s\n",lab);
		return ;
	}
	s=*exps;
	lab=NULL;
	vp=v_find(math_vm,"_start");
	if (!vp)
	{
		dp(".callup: 不能找到预设变量 _start\n");
		goto end;
	}
	start=(char*)vp->v.v_void;
	if (n>0)
	{
		vp=argv(0);
		if (m_leng(vp->mode)==leng_no&&m_type(vp->mode)==type_string)
		lab=vp->v.v_string;
	}
	if (!lab) goto end;
	while(s>start)
	{
		while(s>start&&*s!='\n') s--;
		if (*s=='\n') s++;
		if (*s=='#'&&scmp(s+1,lab)) goto end;
		if (s>start&&*(s-1)=='\n') s--;
		if (s>start&&*s=='\n') s--;
	}
	end:
	*exps=s;
}

_fun(ret)
{
	char *lab;
	var *vp;
	int *sp;
	if (n>0)
	{
		ret->mode=type_int;
		ret->v.v_int=_int(argv(0));
	}
	else
	{
		ret->mode=type_void;
		ret->v.v_void=NULL;
	}
	vp=v_find(math_vm,"_sp");
	if (!vp)
	{
		dp(".ret: 不能找到预设变量 _sp\n");
		return ;
	}
	sp=&(vp->v.v_int);
	if (*sp<=0)
	{
		dp(".ret: _sp 不符合条件 (_sp = %d)\n",*sp);
		return ;
	}
	(*sp)--;
	lab=spget("_s_",*sp);
	vp=v_find(math_vm,lab);
	if (vp)
	{
		*exps=(char*)vp->v.v_void;
		v_free(math_vm,lab);
		return ;
	}
	else
	{
		dp(".ret: 不能找到变量 %s\n",lab);
		return ;
	}
}

_fun(if)
{
	char *s=*exps;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	while(*s&&*s!=';') s++;
	if (n>0)
	{
		var *r=argv(0);
		if (m_leng(r->mode)!=leng_no)
		{
			if (r->v.v_void) goto end;
		}
		else switch(m_type(r->mode))
		{
			case type_int:
				if (r->v.v_int) goto end;
				break;
			case type_long:
				if (r->v.v_long) goto end;
				break;
			case type_float:
				if (r->v.v_float) goto end;
				break;
			default:
				if (r->v.v_void) goto end;
				break;
		}
	}
	if (*s==';') s++;
	while(*s&&*s!=';') s++;
	end:
	*exps=s;
}

_fun(exit)
{
	ret->mode=type_end;
	ret->v.v_int=0;
	if (n>0) ret->v.v_int=_int(argv(0));
	return ;
}

_fun(echo)
{
	char *s;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	while(vlist->l) vlist=vlist->l;
	if (m_leng(vlist->mode)!=leng_no||m_type(vlist->mode)!=type_string)
	{
		dp("echo: %s 不是 string 类型\n",vlist->name);
		return ;
	}
	s=vlist->v.v_string;
	while(*s)
	{
		if (*s=='%') switch(*(++s))
		{
			case '%':
				dp("%");
				break;
			case 'd':
				if (vlist->r)
				{
					vlist=vlist->r;
					dp("%d",_int(vlist));
				}
				break;
			case 'D':
				if (vlist->r)
				{
					vlist=vlist->r;
					dp("%ld",_long(vlist));
				}
				break;
			case 'f':
				if (vlist->r)
				{
					vlist=vlist->r;
					dp("%f",_float(vlist));
				}
				break;
			case 'e':
				if (vlist->r)
				{
					vlist=vlist->r;
					dp("%e",_float(vlist));
				}
				break;
			case 'g':
				if (vlist->r)
				{
					vlist=vlist->r;
					dp("%g",_float(vlist));
				}
				break;
			case 's':
				if (vlist->r)
				{
					vlist=vlist->r;
					if (m_leng(vlist->mode)==leng_no&&m_type(vlist->mode)==type_string) dp("%s",vlist->v.v_string);
				}
				break;
			case 'c':
				if (vlist->r)
				{
					vlist=vlist->r;
					dp("%c",_int(vlist));
				}
				break;
			case 'x':
				if (vlist->r)
				{
					vlist=vlist->r;
					dp("%08x",_int(vlist));
				}
				break;
			case 'X':
				if (vlist->r)
				{
					vlist=vlist->r;
					dp("%016lx",_long(vlist));
				}
				break;
		}
		else dp("%c",*s);
		s++;
	}
}

_fun(set)
{
	char *s,*name;
	int mode;
	var *vp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (call)
	{
		if (m_leng(call->mode)!=leng_no||m_type(call->mode)!=type_object)
		{
			dp(".set: %s 变量不是 object 类型\n",call->name);
			return ;
		}
	}
	while(vlist->l) vlist=vlist->l;
	for(;vlist;vlist=vlist->r)
	{
		if (m_leng(vlist->mode)!=leng_no||m_type(vlist->mode)!=type_string)
		{
			dp(".set: %s 变量不是 string 类型\n");
			continue;
		}
		s=vlist->v.v_string;
		if (scmp_skip(&s,"void")) mode=type_void;
		else if (scmp_skip(&s,"int")) mode=type_int;
		else if (scmp_skip(&s,"long")) mode=type_long;
		else if (scmp_skip(&s,"float")) mode=type_float;
		else if (scmp_skip(&s,"string")) mode=type_string;
		else if (scmp_skip(&s,"object")) mode=type_object;
		else
		{
			dp(".set: 未识别类型 %s\n",sget(s));
			continue;
		}
		if (*s!=':')
		{
			dp(".set: 未找到分隔符 \':\'\n");
			continue;
		}
		s++;
		name=sget(s);
		s=sskip(s);
		if (name[0]==0||name[0]=='_')
		{
			dp(".set: 未找到合法名称(应不以 '_' 开头且长度大于 0)\n");
			continue;
		}
		if (strlen(name)>=12)
		{
			dp(".set: 变量名 %s 过长(超过 11 字节)\n",name);
			continue;
		}
		if (*s=='[')
		{
			var vn;
			cal(s,&vn);
			if (vn.mode==type_float)
			mode|=((int)vn.v.v_float)<<16;
		}
		if (call)
		{
			vp=var_find(call->v.v_object,name);
			if (m_auth_set(call->mode)) goto err_auth;
			if (vp)
			{
				if (m_auth_set(vp->mode))
				{
					err_auth:
					dp(".set: 无权申请变量 %s\n",name);
					continue;
				}
				else call->v.v_object=var_free(call->v.v_object,name);
			}
			vp=var_alloc(call->v.v_object,name,mode,NULL);
			if (vp) call->v.v_object=vp;
			else
			{
				err_alloc:
				dp(".set: 申请变量 %s 失败\n",name);
				continue;
			}
		}
		else
		{
			vp=v_find(math_vm,name);
			if (vp)
			{
				if (m_auth_set(vp->mode)) goto err_auth;
				else v_free(math_vm,name);
			}
			vp=v_alloc(math_vm,name,mode,NULL);
			if (!vp) goto err_alloc;
		}
	}
}

_fun(unset)
{
	char *name;
	int mode;
	var *vp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (call)
	{
		if (m_leng(call->mode)!=leng_no||m_type(call->mode)!=type_object)
		{
			dp(".unset: %s 变量不是 object 类型\n",call->name);
			return ;
		}
	}
	while(vlist->l) vlist=vlist->l;
	for(;vlist;vlist=vlist->r)
	{
		if (m_leng(vlist->mode)!=leng_no||m_type(vlist->mode)!=type_string)
		{
			dp(".unset: %s 变量不是 string 类型\n");
			continue;
		}
		name=vlist->v.v_string;
		if (call)
		{
			vp=var_find(call->v.v_object,name);
			if (vp)
			{
				if (m_auth_set(vp->mode)||m_auth_set(call->mode))
				{
					err_auth:
					dp(".unset: 无权释放变量 %s\n",name);
					continue;
				}
				else call->v.v_object=var_free(call->v.v_object,name);
			}
		}
		else
		{
			vp=v_find(math_vm,name);
			if (vp)
			{
				if (m_auth_set(vp->mode)) goto err_auth;
				else v_free(math_vm,name);
			}
		}
	}
}

// other
_fun(clear)
{
	char *name;
	var *vp,v;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	while(vlist->l) vlist=vlist->l;
	for(;vlist;vlist=vlist->r)
	{
		if (m_leng(vlist->mode)!=leng_no||m_type(vlist->mode)!=type_string)
		{
			dp(".clear: %s 变量不是 string 类型\n");
			continue;
		}
		name=vlist->v.v_string;
		if (name) get_var(name,&vp,&v);
		else continue;
		if (vp)
		{
			if (m_auth_set(vp->mode)) dp(".clear: 无权清除变量 %s\n");
			else value_free(vp);
		}
		else dp(".clear: 不能找到变量 %s\n");
	}
}

_fun(strcpy)
{
	var *vp,v;
	char *name,*src,**aim;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n>1)
	{
		vp=argv(0);
		name=_string(vp);
		vp=argv(1);
		src=_string(vp);
		if (name) get_var(name,&vp,&v);
		else goto err;
		if (m_leng(vp->mode)!=leng_no||m_type(vp->mode)!=type_string)
		{
			dp(".strcpy: 不能找到 string 类型的 %s 变量\n",name);
			return ;
		}
		else if (m_auth_rev(vp->mode))
		{
			dp(".strcpy: 无权修改变量 %s\n",name);
			return ;
		}
		if (m_poin(vp->mode))
		{
			aim=(char**)vp->v.v_void;
			if (*aim) free(*aim);
		}
		else
		{
			aim=&(vp->v.v_string);
			if (m_free(vp->mode))
			{
				free(*aim);
				vp->mode&=~free_need;
			}
		}
		if (src) *aim=malloc(strlen(src)+1);
		else return ;
		if (vp->v.v_string)
		{
			strcpy(*aim,src);
			if (!m_poin(vp->mode)) vp->mode|=free_need;
		}
		else dp(".strcpy: 申请字符串空间失败\n");
		return ;
	}
	err:
	dp(".strcpy: 传递非法参数\n");
}

// math fun
_fun(int)
{
	var *vp;
	ret->mode=type_int;
	if (n>0)
	{
		vp=argv(0);
		if (m_leng(vp->mode)==leng_no) switch(m_type(vp->mode))
		{
			case type_int:
				ret->v.v_int=(int)vp->v.v_int;
				return ;
			case type_long:
				ret->v.v_int=(int)vp->v.v_long;
				return ;
			case type_float:
				ret->v.v_int=(int)vp->v.v_float;
				return ;
		}
	}
	ret->v.v_int=0;
}

_fun(long)
{
	var *vp;
	ret->mode=type_long;
	if (n>0)
	{
		vp=argv(0);
		if (m_leng(vp->mode)==leng_no) switch(m_type(vp->mode))
		{
			case type_int:
				ret->v.v_long=(long long)vp->v.v_int;
				return ;
			case type_long:
				ret->v.v_long=(long long)vp->v.v_long;
				return ;
			case type_float:
				ret->v.v_long=(long long)vp->v.v_float;
				return ;
		}
	}
	ret->v.v_long=0;
}

_fun(float)
{
	var *vp;
	ret->mode=type_float;
	if (n>0)
	{
		vp=argv(0);
		if (m_leng(vp->mode)==leng_no) switch(m_type(vp->mode))
		{
			case type_int:
				ret->v.v_float=(double)vp->v.v_int;
				return ;
			case type_long:
				ret->v.v_float=(double)vp->v.v_long;
				return ;
			case type_float:
				ret->v.v_float=(double)vp->v.v_float;
				return ;
		}
	}
	ret->v.v_float=0;
}

_fun(cal)
{
	var *vp;
	if (n>0)
	{
		vp=argv(0);
		if (m_leng(vp->mode)==leng_no&&m_type(vp->mode)==type_string)
		{
			cal(vp->v.v_string,ret);
			return ;
		}
	}
	ret->mode=type_float;
	ret->v.v_float=0.0;
	dp(".cal: 传入参数错误\n");
}

_fun(sqrt)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=sqrt(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(squa)
{
	double f;
	ret->mode=type_float;
	if (n>0)
	{
		f=_float(argv(0));
		ret->v.v_float=f*f;
	}
	else ret->v.v_float=0;
}

_fun(pow)
{
	ret->mode=type_float;
	if (n>1) ret->v.v_float=pow(_float(argv(0)),_float(argv(1)));
	else ret->v.v_float=0;
}

_fun(exp)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=exp(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(ln)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=log(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(lg)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=log2(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(log)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=log10(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(sin)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=sin(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(cos)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=cos(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(tan)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=tan(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(asin)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=asin(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(acos)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=acos(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(atan)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=atan(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(sinh)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=sinh(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(cosh)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=cosh(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(tanh)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=tanh(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(asinh)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=asinh(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(acosh)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=acosh(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(atanh)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=atanh(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(ceil)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=ceil(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(floor)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=floor(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(abs)
{
	ret->mode=type_float;
	if (n>0) ret->v.v_float=fabs(_float(argv(0)));
	else ret->v.v_float=0;
}

_fun(sign)
{
	double f;
	ret->mode=type_int;
	if (n>0)
	{
		f=_float(argv(0));
		ret->v.v_int=(f>0)?1:(f<0)?-1:0;
	}
	else ret->v.v_int=0;
}

_fun(mod)
{
	ret->mode=type_float;
	if (n>1) ret->v.v_float=fmod(_float(argv(0)),_float(argv(1)));
	else ret->v.v_float=0;
}

_fun(hypot)
{
	ret->mode=type_float;
	if (n>1) ret->v.v_float=hypot(_float(argv(0)),_float(argv(1)));
	else ret->v.v_float=0;
}


_fun(ldexp)
{
	ret->mode=type_float;
	if (n>1) ret->v.v_float=ldexp(_float(argv(0)),_int(argv(1)));
	else ret->v.v_float=0;
}

// debug
_fun(pv)
{
	varmat_print(math_vm);
	ret->mode=type_void;
	ret->v.v_void=NULL;
}

_fun(pg)
{
	varmat_print(glob_vm);
	ret->mode=type_void;
	ret->v.v_void=NULL;
}

_fun(po)
{
	int num;
	var *vp;
	for(num=0;num<n;num++)
	{
		vp=argv(num);
		if (m_leng(vp->mode)||m_type(vp->mode)!=type_object)
		{
			dp("%s 变量不是 object 类型\n",vp->name);
			break;
		}
		else varlist_print(vp->v.v_object);
	}
	ret->mode=type_void;
	ret->v.v_void=NULL;
}


