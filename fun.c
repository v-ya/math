#include "math.h"

char funbuff[funbuff_size];
#define	vlist_max 16
char _vlist_buff[vlist_max*sizeof(void*)];

void vlist_set(va_list *ap, var *vlist)
{
	void *va;
	int i;
	va=_vlist_buff;
	i=0;
	if (sizeof(void*)==4) while(vlist&&i<(vlist_max-1))
	{
		if (m_type2(vlist->mode)==type_long||m_type2(vlist->mode)==type_float)
		{
			((unsigned int*)va)[i]=(unsigned int)(vlist->v.v_long);
			i++;
			((unsigned int*)va)[i]=(unsigned int)(vlist->v.v_long>>32);
			vlist=vlist->r;
			i++;
		}
		else
		{
			((void**)va)[i]=vlist->v.v_void;
			vlist=vlist->r;
			i++;
		}
	}
	else if (sizeof(void*)==8) while(vlist&&(vlist_max-1))
	{
		((void**)va)[i]=vlist->v.v_void;
		vlist=vlist->r;
		i++;
	}
	((void**)va)[i]=NULL;
	// gcc
	#if (defined( __linux__) && defined(__x86_64__))
	(*ap)[0].gp_offset = 48;
	(*ap)[0].fp_offset = 304;
	(*ap)[0].reg_save_area = NULL;
	(*ap)[0].overflow_arg_area = va;
	#else
	(*ap)=va;
	#endif
}

int is_obj(var *obj)
{
	if ((!obj)||m_type2(obj->mode)!=type_object) return 0;
	if (m_auth_rev(obj->mode)) return 0;
	if (!obj->v.v_object) return 0;
	return 1;
}

int obj_type(var *obj, char *name, char *value)
{
	var *vp;
	if ((!obj)||m_type2(obj->mode)!=type_object) return -1;
	vp=var_ralloc(obj->v.v_object,name,type_string|auth_noset|auth_norev,NULL);
	if (!vp) return -1;
	vp->v.v_string=value;
	return 0;
}

int is_obj_type(var *obj, char *name, char *value)
{
	var *vp;
	if ((!obj)||m_type2(obj->mode)!=type_object) return 0;
	vp=var_find(obj->v.v_object,name);
	if (!vp) return 0;
	if (vp->v.v_string&&strcmp(vp->v.v_string,value)==0) return 1;
	return 0;
}

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
		if (m_type2(vp->mode)==type_string)
		lab=vp->v.v_string;
	}
	if (!lab) goto err;
	else if (lab[0]==0) goto err;
	while(*s)
	{
		while(*s&&*s!='\n') s++;
		if (*s) s++;
		if (*s=='#'&&scmp(s+1,lab)) goto end;
	}
	err:
	ret->mode=type_err;
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
		goto err;
	}
	start=(char*)vp->v.v_void;
	if (n>0)
	{
		vp=argv(0);
		if (m_type2(vp->mode)==type_string)
		lab=vp->v.v_string;
	}
	if (!lab) goto err;
	else if (lab[0]==0) goto err;
	while(s>start)
	{
		while(s>start&&*s!='\n') s--;
		if (*s=='\n') s++;
		if (*s=='#'&&scmp(s+1,lab)) goto end;
		if (s>start&&*(s-1)=='\n') s--;
		if (s>start&&*s=='\n') s--;
	}
	err:
	ret->mode=type_err;
	dp(".jup 不能找到标签 %s\n",lab);
	return ;
	end:
	*exps=s;
}

_fun(ldw_test)
{
	char *s=*exps,*lab=NULL;
	var *vp;
	ret->mode=type_int;
	ret->v.v_int=0;
	if (n>0)
	{
		vp=argv(0);
		if (m_type2(vp->mode)==type_string)
		lab=vp->v.v_string;
	}
	if (!lab) return ;
	else if (lab[0]==0) return ;
	while(*s)
	{
		while(*s&&*s!='\n') s++;
		if (*s) s++;
		if (*s=='#'&&scmp(s+1,lab)) goto end;
	}
	return ;
	end:
	ret->v.v_int=1;
}

_fun(lup_test)
{
	char *s=*exps,*lab=NULL,*start;
	var *vp;
	ret->mode=type_int;
	ret->v.v_int=0;
	vp=v_find(math_vm,"_start");
	if (!vp)
	{
		dp(".jup: 不能找到预设变量 _start\n");
		ret->mode=type_err;
		return ;
	}
	start=(char*)vp->v.v_void;
	if (n>0)
	{
		vp=argv(0);
		if (m_type2(vp->mode)==type_string)
		lab=vp->v.v_string;
	}
	if (!lab) return ;
	else if (lab[0]==0) return ;
	while(s>start)
	{
		while(s>start&&*s!='\n') s--;
		if (*s=='\n') s++;
		if (*s=='#'&&scmp(s+1,lab)) goto end;
		if (s>start&&*(s-1)=='\n') s--;
		if (s>start&&*s=='\n') s--;
	}
	return ;
	end:
	ret->v.v_int=1;
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
		ret->mode=type_err;
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
		ret->mode=type_err;
		return ;
	}
	s=*exps;
	lab=NULL;
	if (n>0)
	{
		vp=argv(0);
		if (m_type2(vp->mode)==type_string)
		lab=vp->v.v_string;
	}
	if (!lab) goto err;
	else if (lab[0]==0) goto err;
	while(*s)
	{
		while(*s&&*s!='\n') s++;
		if (*s) s++;
		if (*s=='#'&&scmp(s+1,lab)) goto end;
	}
	err:
	ret->mode=type_err;
	return ;
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
		ret->mode=type_err;
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
		ret->mode=type_err;
		return ;
	}
	s=*exps;
	lab=NULL;
	vp=v_find(math_vm,"_start");
	if (!vp)
	{
		dp(".callup: 不能找到预设变量 _start\n");
		ret->mode=type_err;
		return ;
	}
	start=(char*)vp->v.v_void;
	if (n>0)
	{
		vp=argv(0);
		if (m_type2(vp->mode)==type_string)
		lab=vp->v.v_string;
	}
	if (!lab) goto err;
	else if (lab[0]==0) goto err;
	while(s>start)
	{
		while(s>start&&*s!='\n') s--;
		if (*s=='\n') s++;
		if (*s=='#'&&scmp(s+1,lab)) goto end;
		if (s>start&&*(s-1)=='\n') s--;
		if (s>start&&*s=='\n') s--;
	}
	err:
	ret->mode=type_err;
	return ;
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
		ret->mode=type_err;
		return ;
	}
	sp=&(vp->v.v_int);
	if (*sp<=0)
	{
		dp(".ret: _sp 不符合条件 (_sp = %d)\n",*sp);
		ret->mode=type_err;
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
		ret->mode=type_err;
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
	va_list ap;
	char *s;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	while(vlist->l) vlist=vlist->l;
	if (m_type2(vlist->mode)!=type_string)
	{
		dp("echo: %s 不是 string 类型\n",vlist->name);
		ret->mode=type_err;
		return ;
	}
	s=vlist->v.v_string;
	vlist=vlist->r;
	vlist_set(&ap,vlist);
	vsnprintf(funbuff,funbuff_size,s,ap);
	dp(funbuff);
	return ;
}

_fun(set)
{
	char *s,name[32];
	int mode;
	var *vp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n==0) return ;
	if (call)
	{
		if (m_type2(call->mode)!=type_object)
		{
			dp(".set: %s 变量不是 object 类型\n",call->name);
			ret->mode=type_err;
			return ;
		}
	}
	while(vlist->l) vlist=vlist->l;
	for(;vlist;vlist=vlist->r)
	{
		if (m_type2(vlist->mode)!=type_string)
		{
			dp(".set: %s 变量不是 string 类型\n",vlist->name);
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
		strcpy(name,sget(s));
		s=sskip(s);
		if (name[0]==0)
		{
			dp(".set: 未找到合法名称\n");
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
			if (vn.v.v_float>0) mode|=((int)vn.v.v_float)<<16;
		}
		if (call)
		{
			if (m_auth_rev(call->mode)) goto err_auth;
			vp=var_find(call->v.v_object,name);
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
	if (n==0) return ;
	if (call)
	{
		if (m_type2(call->mode)!=type_object)
		{
			dp(".unset: %s 变量不是 object 类型\n",call->name);
			ret->mode=type_err;
			return ;
		}
	}
	while(vlist->l) vlist=vlist->l;
	for(;vlist;vlist=vlist->r)
	{
		if (m_type2(vlist->mode)!=type_string)
		{
			dp(".unset: %s 变量不是 string 类型\n",vlist->name);
			continue;
		}
		name=vlist->v.v_string;
		if (call)
		{
			if (m_auth_rev(call->mode)) goto err_auth;
			vp=var_find(call->v.v_object,name);
			if (vp)
			{
				if (m_auth_set(vp->mode))
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

_fun(func)
{
	char *s,*sb,*code,name[32];
	int mode;
	var *vp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n==0) return ;
	while(vlist->l) vlist=vlist->l;
	if (m_type2(vlist->mode)!=type_string)
	{
		dp(".func: %s 变量不是 string 类型\n",vlist->name);
		ret->mode=type_err;
		return ;
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
		dp(".func: 未识别类型 %s\n",sget(s));
		ret->mode=type_err;
		return ;
	}
	if (*s!=':')
	{
		dp(".func: 未找到分隔符 \':\'\n");
		ret->mode=type_err;
		return ;
	}
	s++;
	strcpy(name,sget(s));
	s=sskip(s);
	if (name[0]==0)
	{
		dp(".func: 未找到合法名称\n");
		ret->mode=type_err;
		return ;
	}
	if (strlen(name)>=12)
	{
		dp(".func: 变量名 %s 过长(超过 11 字节)\n",name);
		ret->mode=type_err;
		return ;
	}
	if (*s=='[')
	{
		var vn;
		cal(s,&vn);
		if (vn.v.v_float>0) mode|=((int)vn.v.v_float)<<16;
	}
	vp=v_find(math_vm,name);
	if (vp)
	{
		if (m_auth_set(vp->mode))
		{
			dp(".func: 无权申请变量 %s\n",name);
			ret->mode=type_err;
			return ;
		}
		else v_free(math_vm,name);
	}
	vp=v_alloc(math_vm,name,type_object|func_code,NULL);
	call=NULL;
	if (!vp)
	{
		err_alloc:
		if (call) v_free(math_vm,name);
		dp(".func: 申请变量 %s 失败\n",name);
		ret->mode=type_err;
		return ;
	}
	call=vp;
	vp=var_alloc(call->v.v_object,"ret",mode|auth_noset,NULL);
	if (!vp) goto err_alloc;
	vp=var_alloc(call->v.v_object,"argc",type_int|auth_noset,NULL);
	if (!vp) goto err_alloc;
	vp=var_alloc(call->v.v_object,"argv",type_void,NULL);
	if (!vp) goto err_alloc;
	vp->mode=type_object|auth_noset|auth_norev;
	vp=var_alloc(call->v.v_object,"code",type_void|auth_noset|auth_norev,NULL);
	if (!vp) goto err_alloc;
	s=*exps;
	while(is_space(*s)) s++;
	if (*s==';') s++;
	while(is_space(*s)) s++;
	if (*s=='{')
	{
		*s='#';
		s++;
		sb=s;
		mode=1;
		while(*s&&mode>0)
		{
			if (*s=='\"') s=str_skip(s);
			else if (*s=='{')
			{
				mode++;
				s++;
			}
			else if (*s=='}')
			{
				mode--;
				if (mode>0) s++;
			}
			else s++;
		}
		if (*s=='}') *s='\n';
		code=malloc(s-sb+1);
		if (!code) goto err_alloc;
		vp->v.v_void=code;
		vp->mode|=free_need;
		while(sb<s)
		{
			*(code++)=*sb;
			if (*sb=='\n') *sb=' ';
			sb++;
		}
		*code=0;
	}
}

// other
_fun(test)
{
	
	var *vp,*vt,v,v2;
	int px;
	char *name;
	ret->mode=type_int;
	ret->v.v_int=0;
	if (n>0)
	{
		vp=argv(0);
		name=_string(vp);
		if (name)
		{
			if (name[0]=='.')
			{
				name++;
				if (is_Name(*name)) vp=v_find(glob_vm,sget(name));
				else return ;
			}
			else if (is_Name(*name)) vp=v_find(math_vm,sget(name));
			else return ;
			name=sskip(name);
			if (!vp) return ;
			while(*name)
			{
				switch(*name)
				{
					case '.':
						name++;
						if (m_type2(vp->mode)==type_object) vp=var_find(vp->v.v_object,sget(name));
						else return ;
						name=sskip(name);
						break;
					case '[':
						name=cal(name,&v2);
						px=(int)v2.v.v_float;
						if (px<0||px>=m_leng(vp->mode)) return ;
						v.mode=vp->mode&0x0000ffff;
						if (m_type(v.mode)==type_object) v.v.v_object=((var**)(vp->v.v_void))[px];
						else v.v.v_void=NULL;
						vp=&v;
						break;
					default:
						return ;
				}
				if (!vp) return ;
			}
		}
		else return ;
		if (n==1) goto is_ex;
		vt=argv(1);
		name=_string(vt);
		if (!name) return ;
		switch(m_type(vp->mode))
		{
			case type_void:
				if (m_leng(vp->mode))
				{
					if (strcmp(name,"void[]")==0) goto is_ex;
					else return ;
				}
				else if (strcmp(name,"void")==0) goto is_ex;
				else if (m_func(vp->mode)&&strcmp(name,"func")==0) goto is_ex;
				return ;
			case type_int:
				if (m_leng(vp->mode))
				{
					if (strcmp(name,"int[]")==0) goto is_ex;
					else return ;
				}
				else if (strcmp(name,"int")==0) goto is_ex;
				return ;
			case type_long:
				if (m_leng(vp->mode))
				{
					if (strcmp(name,"long[]")==0) goto is_ex;
					else return ;
				}
				else if (strcmp(name,"long")==0) goto is_ex;
				return ;
			case type_float:
				if (m_leng(vp->mode))
				{
					if (strcmp(name,"float[]")==0) goto is_ex;
					else return ;
				}
				else if (strcmp(name,"float")==0) goto is_ex;
				return ;
			case type_string:
				if (m_leng(vp->mode))
				{
					if (strcmp(name,"string[]")==0) goto is_ex;
					else return ;
				}
				else if (strcmp(name,"string")==0) goto is_ex;
				return ;
			case type_object:
				if (m_leng(vp->mode))
				{
					if (strcmp(name,"object[]")==0) goto is_ex;
					else return ;
				}
				else if (strcmp(name,"object")==0) goto is_ex;
				return ;
		}
	}
	is_ex:
	ret->v.v_int=1;
}

_fun(clear)
{
	char *name;
	var *vp,v;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	while(vlist->l) vlist=vlist->l;
	for(;vlist;vlist=vlist->r)
	{
		if (m_type2(vlist->mode)!=type_string)
		{
			dp(".clear: %s 变量不是 string 类型\n",vlist->name);
			continue;
		}
		name=vlist->v.v_string;
		if (name) get_var(name,&vp,&v);
		else continue;
		if (vp)
		{
			int i;
			if (m_auth_rev(vp->mode)) dp(".clear: 无权清除变量 %s\n",name);
			if (m_poin(vp->mode))
			{
				v.mode=vp->mode&0x0000ffff&(~type_pointer)|0x00010000;
				v.v.v_void=vp->v.v_void;
				vp=&v;
			}
			switch(m_type(vp->mode))
			{
				case type_void:
					if (m_leng(vp->mode)) for(i=m_leng(vp->mode)-1;i>=0;i--)
					{
						((void **)(vp->v.v_void))[i]=NULL;
					}
					else vp->v.v_void=NULL;
					return ;
				case type_int:
					if (m_leng(vp->mode)) for(i=m_leng(vp->mode)-1;i>=0;i--)
					{
						((int *)(vp->v.v_void))[i]=0;
					}
					else vp->v.v_int=0;
					return ;
				case type_long:
					if (m_leng(vp->mode)) for(i=m_leng(vp->mode)-1;i>=0;i--)
					{
						((long long *)(vp->v.v_void))[i]=0;
					}
					else vp->v.v_long=0;
					return ;
				case type_float:
					if (m_leng(vp->mode)) for(i=m_leng(vp->mode)-1;i>=0;i--)
					{
						((double *)(vp->v.v_void))[i]=0;
					}
					else vp->v.v_float=0;
					return ;
				case type_string:
					if (m_leng(vp->mode)) for(i=m_leng(vp->mode)-1;i>=0;i--)
					{
						if (((char **)(vp->v.v_void))[i]) free(((char **)(vp->v.v_void))[i]);
						((char **)(vp->v.v_void))[i]=NULL;
					}
					else
					{
						if (vp->v.v_string) free(vp->v.v_string);
						vp->v.v_string=NULL;
						vp->mode&=~free_need;
					}
					return ;
				case type_object:
					if (m_leng(vp->mode)) for(i=m_leng(vp->mode)-1;i>=0;i--)
					{
						var *vl;
						if (((var **)(vp->v.v_void))[i])
						{
							var *vl,*vt;
							for (vl=((var **)(vp->v.v_void))[i];vl->l;vl=vl->l);
							while(vl)
							{
								vt=vl->r;
								if (strcmp(vl->name,"set")==0||strcmp(vl->name,"unset")==0) ;
								else ((var **)(vp->v.v_void))[i]=var_free(vl,vl->name);
								vl=vt;
							}
						}
					}
					else
					{
						var *vl;
						if (vp->v.v_object)
						{
							var *vl,*vt;
							for (vl=vp->v.v_object;vl->l;vl=vl->l);
							while(vl)
							{
								vt=vl->r;
								if (strcmp(vl->name,"set")==0||strcmp(vl->name,"unset")==0) ;
								else vp->v.v_object=var_free(vl,vl->name);
								vl=vt;
							}
						}
					}
					return ;
			}
		}
		else dp(".clear: 不能找到变量 %s\n",name);
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
		if (m_type2(vp->mode)!=type_string)
		{
			dp(".strcpy: 不能找到 string 类型的 %s 变量\n",name);
			ret->mode=type_err;
			return ;
		}
		else if (m_auth_rev(vp->mode))
		{
			dp(".strcpy: 无权修改变量 %s\n",name);
			ret->mode=type_err;
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
		*aim=NULL;
		if (src) *aim=malloc(strlen(src)+1);
		else return ;
		if (*aim)
		{
			strcpy(*aim,src);
			if (!m_poin(vp->mode)) vp->mode|=free_need;
		}
		else
		{
			ret->mode=type_err;
			dp(".strcpy: 申请字符串空间失败\n");
		}
		return ;
	}
	err:
	ret->mode=type_err;
	dp(".strcpy: 传递非法参数\n");
}

_fun(sprintf)
{
	va_list ap;
	var *vp,v;
	char *name,*src,**aim;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<2) goto err;
	while(vlist->l) vlist=vlist->l;
	name=_string(vlist);
	vlist=vlist->r;
	src=_string(vlist);
	if (name) get_var(name,&vp,&v);
	else goto err;
	if (m_type2(vp->mode)!=type_string)
	{
		dp(".sprintf: 不能找到 string 类型的 %s 变量\n",name);
		ret->mode=type_err;
		return ;
	}
	else if (m_auth_rev(vp->mode))
	{
		dp(".sprintf: 无权修改变量 %s\n",name);
		ret->mode=type_err;
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
	*aim=NULL;
	if (src)
	{
		vlist=vlist->r;
		vlist_set(&ap,vlist);
		vsnprintf(funbuff,funbuff_size,src,ap);
		if (src) *aim=malloc(strlen(src)+1);
		else return ;
		*aim=malloc(strlen(funbuff)+1);
		if (*aim)
		{
			strcpy(*aim,funbuff);
			if (!m_poin(vp->mode)) vp->mode|=free_need;
		}
		else
		{
			ret->mode=type_err;
			dp(".sprintf: 申请字符串空间失败\n");
		}
		return ;
	}
	return ;
	err:
	ret->mode=type_err;
	dp(".sprintf: 传递非法参数\n");
}

_fun(strcmp)
{
	var *vp,v;
	char *s1,*s2;
	ret->mode=type_int;
	ret->v.v_int=0;
	if (n>1)
	{
		vp=argv(0);
		s1=_string(vp);
		vp=argv(1);
		s2=_string(vp);
		if (s1&&s2) ret->v.v_int=strcmp(s1,s2);
		else if (s1) ret->v.v_int=s1[0];
		else if (s2) ret->v.v_int=-s2[0];
	}
}

_fun(strget)
{
	
	var *vp,v;
	char *name,**aim;
	int i;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n>0)
	{
		vp=argv(0);
		name=_string(vp);
		if (name) get_var(name,&vp,&v);
		else goto err;
		if (m_type2(vp->mode)!=type_string)
		{
			dp(".strget: 不能找到 string 类型的 %s 变量\n",name);
			ret->mode=type_err;
			return ;
		}
		else if (m_auth_rev(vp->mode))
		{
			dp(".strget: 无权修改变量 %s\n",name);
			ret->mode=type_err;
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
		*aim=NULL;
		for(i=0;i<funbuff_size;i++)
		{
			funbuff[i]=getchar();
			if (funbuff[i]=='\n')
			{
				funbuff[i]=0;
				break;
			}
		}
		funbuff[funbuff_size-1]=0;
		*aim=malloc(strlen(funbuff)+1);
		if (*aim)
		{
			strcpy(*aim,funbuff);
			if (!m_poin(vp->mode)) vp->mode|=free_need;
		}
		else
		{
			ret->mode=type_err;
			dp(".strget: 申请字符串空间失败\n");
		}
		return ;
	}
	err:
	ret->mode=type_err;
	dp(".strget: 传递非法参数\n");
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
		if (m_type2(vp->mode)==type_string)
		{
			cal(vp->v.v_string,ret);
			return ;
		}
	}
	ret->mode=type_float;
	ret->v.v_float=0.0;
	dp(".cal: 传入参数错误\n");
}

_fun(srand)
{
	ret->mode=type_int;
	if (n==0) ret->v.v_int=time(NULL);
	else ret->v.v_int=_int(argv(0));
	srand(ret->v.v_int);
}

_fun(rand)
{
	var *vp;
	double r;
	if (n==0) r=1;
	else r=_float(argv(0));
	ret->mode=type_float;
	ret->v.v_float=r*rand()/RAND_MAX;
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

_fun(radian)
{
	double x,y;
	ret->mode=type_float;
	if (n>1)
	{
		x=_float(argv(0));
		y=_float(argv(1));
		if (x==0) ret->v.v_float=y>0?M_PI_2:y<0?(-M_PI_2):0;
		else if (x>0) ret->v.v_float=atan(y/x);
		else
		{
			ret->v.v_float=atan(y/x);
			if (ret->v.v_float>0) ret->v.v_float-=M_PI;
			else ret->v.v_float+=M_PI;
		}
	}
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
		if (m_type2(vp->mode)!=type_object)
		{
			dp(".d.po: %s 变量不是 object 类型\n",vp->name);
			break;
		}
		else varlist_print(vp->v.v_object);
	}
	ret->mode=type_void;
	ret->v.v_void=NULL;
}


