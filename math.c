#include "math.h"

varmat *math_vm=NULL;
varmat *glob_vm=NULL;
var *_this_=NULL;

int glob_init(void)
{
	value v;
	var *vp;
	glob_vm=varmat_alloc();
	#define	set_fun(f) v.v_void=&_func_##f;v_alloc(glob_vm,#f,type_void|auth_noset|auth_norev|func_code,&v)
	set_fun(var);
	set_fun(jdw);
	set_fun(jup);
	set_fun(ldw_test);
	set_fun(lup_test);
	set_fun(calldw);
	set_fun(callup);
	set_fun(ret);
	set_fun(if);
	set_fun(exit);
	set_fun(echo);
	set_fun(set);
	set_fun(unset);
	set_fun(func);
	// other
	set_fun(void);
	set_fun(test);
	set_fun(clear);
	set_fun(strcpy);
	set_fun(sprintf);
	set_fun(strcmp);
	set_fun(strget);
	
	// math fun
	set_fun(int);
	set_fun(long);
	set_fun(float);
	set_fun(cal);
	set_fun(srand);
	set_fun(rand);
	set_fun(sqrt);
	set_fun(squa);
	set_fun(pow);
	set_fun(exp);
	set_fun(ln);
	set_fun(lg);
	set_fun(log);
	set_fun(sin);
	set_fun(cos);
	set_fun(tan);
	set_fun(asin);
	set_fun(acos);
	set_fun(atan);
	set_fun(sinh);
	set_fun(cosh);
	set_fun(tanh);
	set_fun(asinh);
	set_fun(acosh);
	set_fun(atanh);
	set_fun(ceil);
	set_fun(floor);
	set_fun(abs);
	set_fun(sign);
	set_fun(mod);
	set_fun(hypot);
	set_fun(radian);
	set_fun(ldexp);
	
	// this
	vp=v_alloc(glob_vm,"this",type_void,NULL);
	vp->mode=type_object|auth_noset|auth_norev|func_code;
	_this_=vp;
	// const var
	vp=v_alloc(glob_vm,"const",type_object|auth_noset|auth_norev,NULL);
	#define	set_var(_v,_n) v.v_float=_v;vp->v.v_void=var_alloc((var*)vp->v.v_void,#_n,type_float|auth_noset|auth_norev,&v)
	set_var(M_E,e);
	set_var(M_LOG2E,lge);
	set_var(M_LOG10E,loge);
	set_var(M_LN2,ln2);
	set_var(M_LN10,ln10);
	set_var(M_PI,pi);
	set_var(M_PI_2,pi_2);
	set_var(M_PI_4,pi_4);
	set_var(M_1_PI,_1_pi);
	set_var(M_2_PI,_2_pi);
	set_var(M_2_SQRTPI,_2_sqrtpi);
	set_var(M_SQRT2,sqrt2);
	set_var(M_SQRT1_2,sqrt1_2);
	#undef	set_var
	
	#undef	set_fun
	#define	set_fun(f) v.v_void=&_func_##f;vp->v.v_void=var_alloc((var*)vp->v.v_void,#f,type_void|auth_noset|auth_norev|func_code,&v)
	// import
	vp=v_alloc(glob_vm,"import",type_object|auth_noset|auth_norev,NULL);
	set_fun(calc_init);
	set_fun(bmp_init);
	set_fun(wav_init);
	set_fun(avi_init);
	
	vp=v_alloc(glob_vm,"d",type_object|auth_noset|auth_norev,NULL);
	set_fun(pv);
	set_fun(pg);
	set_fun(po);
	#undef	set_fun
	return 0;
}

char* get_varlist(char *exp, int *n, var **vl)
{
	var *vp,v,*vlist;
	char *s;
	*n=0;
	*vl=NULL;
	while(*exp&&*exp!=';'&&*exp!=')')
	{
		s=exp+1;
		while(is_space(*s)) s++;
		if (is_Name(*s)||*s=='.')
		{
			exp=get_var(s,&vp,&v);
		}
		/*else if (*s=='[')
		{
			
		}
		else if (*s=='{')
		{
			
		}*/
		else if (*s=='\"')
		{
			v.mode=type_string|auth_noset|auth_norev;
			exp=s;
			v.v.v_string=str_alloc(&exp);
			if (!(v.v.v_string)) goto err;
			vp=&v;
		}
		else if (*s==')')
		{
			exp=s;
			goto end;
		}
		else
		{
			exp=cal(exp,&v);
			vp=&v;
		}
		sprintf(v.name,"_%d",*n);
		if (m_poin(vp->mode))
		{
			v.mode=m_type(vp->mode)|auth_noset;
			switch(m_type(v.mode))
			{
				case type_void:
					v.v.v_void=*((void**)(vp->v.v_void));
					break;
				case type_int:
					v.v.v_int=*((int*)(vp->v.v_void));
					break;
				case type_long:
					v.v.v_long=*((long long*)(vp->v.v_void));
					break;
				case type_float:
					v.v.v_float=*((double*)(vp->v.v_void));
					break;
				case type_string:
					v.v.v_string=*((char**)(vp->v.v_void));
					break;
				case type_object:
					v.v.v_object=*((var**)(vp->v.v_void));
					break;
				default:
					v.v.v_void=NULL;
			}
			vp=&v;
		}
		if (m_leng(vp->mode)||m_type(vp->mode)==type_string||m_type(vp->mode)==type_object)
		{
			vlist=var_alloc(*vl,v.name,type_void,NULL);
			if (!vlist) goto err;
			vlist->mode=(vp->mode)&(~free_need)|auth_noset;
			vlist->v.v_void=vp->v.v_void;
		}
		else
		{
			vlist=var_alloc(*vl,v.name,vp->mode|auth_noset,&(vp->v));
			if (!vlist) goto err;
		}
		(*n)++;
		*vl=vlist;
		while(is_space(*exp)) exp++;
	}
	end:
	if (*exp==')') exp++;
	return exp;
	err:
	dp("get_varlist: 申请变量失败\n");
	*n=-1;
	var_listfree(*vl);
	*vl=NULL;
	return exp;
}

char* run_func(char *exp, var **vp)
{
	var *vl,*list;
	int _argc,mode,n;
	var *_argv,*this;
	char *code;
	exp=get_varlist(exp,&n,&list);
	this=_this_->v.v_object;
	if (this)
	{
		vl=var_find(this,"argc");
		_argc=vl->v.v_int;
		vl=var_find(this,"argv");
		_argv=vl->v.v_object;
	}
	vl=var_find((*vp)->v.v_object,"code");
	_this_->v.v_object=vl;
	code=(char*)vl->v.v_void;
	mode=(*vp)->mode;
	(*vp)->mode|=auth_noset;
	vl=var_find(_this_->v.v_object,"argc");
	vl->v.v_int=n;
	vl=var_find(_this_->v.v_object,"argv");
	vl->v.v_object=list;
	math_run(code,NULL,0,NULL);
	var_listfree(list);
	vl->v.v_object=NULL;
	_this_->v.v_object=this;
	if (this)
	{
		vl=var_find(this,"argc");
		vl->v.v_int=_argc;
		vl=var_find(this,"argv");
		vl->v.v_object=_argv;
	}
	(*vp)->mode=mode;
	*vp=var_find((*vp)->v.v_object,"ret");
	return exp;
}

char* get_var(char *exp, var **vp, var *v)
{
	var *vl,*vc;
	char *name;
	if (*exp=='.')
	{
		exp++;
		name=sget(exp);
		exp=sskip(exp);
		vl=v_find(glob_vm,name);
	}
	else if (is_Name(*exp))
	{
		name=sget(exp);
		exp=sskip(exp);
		vl=v_find(math_vm,name);
		if (!vl) vl=v_find(glob_vm,name);
	}
	else
	{
		double r;
		r=0;
		*vp=v;
		if (*exp=='(')
		{
			exp=cal(exp,v);
			if (*exp==')') exp++;
			return exp;
		}
		while(*exp>='0'&&*exp<='9')
		{
			r=r*10+(*exp-'0');
			exp++;
		}
		v->mode=type_float;
		v->v.v_float=r;
		if (*exp!='.') return exp;
		exp++;
		r=1.0;
		while(*exp>='0'&&*exp<='9')
		{
			r/=10;
			v->v.v_float+=r*(*exp-'0');
			exp++;
		}
		return exp;
	}
	vc=NULL;
	_get_var_loop:
	if (!vl) goto err_find;
	while(is_space(*exp)) exp++;
	if (*exp=='.')
	{
		vc=vl;
		if ((m_leng(vl->mode)!=leng_no)||((m_type(vl->mode)!=type_object)&0xf)) goto err_obj;
		exp++;
		name=sget(exp);
		exp=sskip(exp);
		if (m_poin(vl->mode)) vl=var_find(*((var**)(vc->v.v_void)),name);
		else vl=var_find(vc->v.v_object,name);
		if (!vl) goto err_find;
		goto _get_var_loop;
	}
	else if (*exp=='(')
	{
		if (!m_func(vl->mode)) goto err_func;
		if (m_type(vl->mode)==type_void)
		{
			int n;
			func f;
			if (m_poin(vl->mode)) vl->v.v_void=*((void**)(vl->v.v_void));
			f=(func) vl->v.v_void;
			exp=get_varlist(exp,&n,&vl);
			(*f)(&exp,v,vc,n,vl);
			var_listfree(vl);
			vl=v;
		}
		else if (m_type(vl->mode)==type_object) exp=run_func(exp,&vl);
		else goto err_func;
		goto _get_var_loop;
	}
	else if (*exp=='[')
	{
		int px;
		var vv;
		if ((m_leng(vl->mode))==leng_no) goto err_noarr;
		exp=cal(exp,&vv);
		px=(int)vv.v.v_float;
		if (px<0||px>=m_leng(vl->mode)) goto err_szarr;
		if (m_poin(vl->mode)) v->v.v_void=*((void**)(vl->v.v_void));
		else v->v.v_void=vl->v.v_void;
		v->mode=(vl->mode&0xffff)|type_pointer;
		switch(m_type(vl->mode))
		{
			case type_void:
				v->v.v_void=(void*)(((void**)(v->v.v_void))+px);
				break;
			case type_int:
				v->v.v_void=(void*)(((int*)(v->v.v_void))+px);
				break;
			case type_long:
				v->v.v_void=(void*)(((long long*)(v->v.v_void))+px);
				break;
			case type_float:
				v->v.v_void=(void*)(((double*)(v->v.v_void))+px);
				break;
			case type_string:
				v->v.v_void=(void*)(((char**)(v->v.v_void))+px);
				break;
			case type_object:
				v->v.v_void=(void*)(((var**)(v->v.v_void))+px);
				break;
			default:
				dp("get_var: 未知类型数组 %s\n",name);
				goto err;
		}
		vl=v;
		goto _get_var_loop;
	}
	*vp=vl;
	return exp;
	err_find:
	dp("get_var: 不能找到 %s 变量\n",name);
	goto err;
	err_obj:
	dp("get_var: 变量 %s 非 object 类型\n",name);
	goto err;
	err_noarr:
	dp("get_var: 变量 %s 非数组\n",name);
	goto err;
	err_szarr:
	dp("get_var: 数组 %s 引用超限\n",name);
	goto err;
	err_func:
	dp("get_var: 变量 %s 不是函数\n",name);
	goto err;
	err:
	v->mode=type_err;
	v->v.v_void=NULL;
	*vp=v;
	return exp;
}

char* get_float(char *exp, double *r)
{
	var vp;
	var *vl;
	exp=get_var(exp,&vl,&vp);
	switch(m_type(vl->mode))
	{
		case type_int:
			if (m_poin(vl->mode)) *r=*((int*)(vl->v.v_void));
			else *r=vl->v.v_int;
			break;
		case type_long:
			if (m_poin(vl->mode)) *r=*((long long*)(vl->v.v_void));
			else *r=vl->v.v_long;
			break;
		case type_float:
			if (m_poin(vl->mode)) *r=*((double*)(vl->v.v_void));
			else *r=vl->v.v_float;
			break;
		case type_void:
			*r=0;
			break;
		default:
			dp("get_float: 获取的变量不能转换成数值\n");
			*r=0;
			break;
	}
	return exp;
}

char* cal(char *exp, var *v)
{
	var vtmp,*vl;
	int c=0;
	double r,t;
	r=0;
	while(is_space(*exp)) exp++;
	if (*exp==',')
	{
		c=':';
		goto _start;
	}
	while(*exp)
	{
		c=*exp;
		if (c==')'||c==']'||c==';'||c==','||c==0) goto end;
		_start:
		exp++;
		while(is_space(*exp)) exp++;
		switch(c) {
			case ':':
			case '(':
			case '[':
				exp=get_float(exp,&t);
				r=t;
				break;
			case '+':
				exp=get_float(exp,&t);
				r+=t;
				break;
			case '-':
				exp=get_float(exp,&t);
				r-=t;
				break;
			case '*':
				exp=get_float(exp,&t);
				r*=t;
				break;
			case '/':
				exp=get_float(exp,&t);
				r/=t;
				break;
			case '=':
				exp=get_float(exp,&t);
				r=(r==t)?1:0;
				break;
			case '>':
				if (exp[0]=='=')
				{
					exp++;
					while(is_space(*exp)) exp++;
					exp=get_float(exp,&t);
					r=(r>=t)?1:0;
					break;
				}
				else
				{
					exp=get_float(exp,&t);
					r=(r>t)?1:0;
					break;
				}
			case '<':
				if (exp[0]=='=')
				{
					exp++;
					while(is_space(*exp)) exp++;
					exp=get_float(exp,&t);
					r=(r<=t)?1:0;
					break;
				}
				else
				{
					exp=get_float(exp,&t);
					r=(r<t)?1:0;
					break;
				}
			case '&':
				exp=get_float(exp,&t);
				r=((r!=0)&&(t!=0))?1:0;
				break;
			case '|':
				exp=get_float(exp,&t);
				r=((r==0)&&(t==0))?0:1;
				break;
			case '!':
				if (exp[0]=='=')
				{
					exp++;
					while(is_space(*exp)) exp++;
					exp=get_float(exp,&t);
					r=(r!=t)?1:0;
					break;
				}
				else
				{
					exp=get_float(exp,&t);
					r=(t==0)?1:0;
					break;
				}
			case '@':
				exp=get_var(exp,&vl,&vtmp);
				if (m_auth_rev(vl->mode))
				{
					dp("cal: 无权对 %s 进行赋值操作\n",vl->name);
					break;
				}
				switch(m_type(vl->mode))
				{
					case type_int:
						if (m_poin(vl->mode)) *((int*)vl->v.v_void)=(int)r;
						else vl->v.v_int=(int)r;
						break;
					case type_long:
						if (m_poin(vl->mode)) *((long long*)vl->v.v_void)=(long long)r;
						else vl->v.v_long=(long long)r;
						break;
					case type_float:
						if (m_poin(vl->mode)) *((double*)vl->v.v_void)=(double)r;
						else vl->v.v_float=(double)r;
						break;
					default:
						vtmp.name[0]=0;
						dp("cal: 不支持对 %s 的类型进行赋值操作\n",vl->name);
						break;
				}
				break;
			case '?':
				if (r==0)
				{
					c=0;
					while(*exp&&*exp!=';')
					{
						if (*exp=='('||*exp=='[') c++;
						else if (*exp==')'||*exp==']') c--;
						else if (*exp==':'&&c==0)
						{
							exp++;
							while(is_space(*exp)) exp++;
							break;
						}
						if (c<0) break;
						exp++;
					}
				}
				exp=get_float(exp,&r);
				break;
			case '$':
				c=0;
				while(*exp&&*exp!=';')
				{
					if (*exp=='('||*exp=='[') c++;
					else if (*exp==')'||*exp==']') c--;
					else if (*exp==':'&&c==0)
					{
						exp++;
						while(is_space(*exp)) exp++;
						break;
					}
					if (c<0) break;
					exp++;
				}
				c=0;
				while(*exp&&*exp!=';')
				{
					if (*exp=='('||*exp=='[') c++;
					else if (*exp==')'||*exp==']') c--;
					else if (*exp==':'&&c==0)
					{
						exp++;
						while(is_space(*exp)) exp++;
						break;
					}
					if (c<0) break;
					exp++;
				}
				exp=get_float(exp,&r);
		}
		while(is_space(*exp)) exp++;
	}
	end:
	if (*exp==']'||*exp==';') exp++;
	v->mode=type_float;
	v->v.v_float=r;
	return exp;
}

int math_run_save(var **vl)
{
	var *v;
	*vl=NULL;
	if (v=v_find(math_vm,"_start"))
	{
		if (!(v=var_alloc(*vl,v->name,v->mode,&(v->v)))) goto err;
		*vl=v;
		v_free(math_vm,"_start");
	}
	if (v=v_find(math_vm,"_ret"))
	{
		if (!(v=var_alloc(*vl,v->name,v->mode,&(v->v)))) goto err;
		*vl=v;
		v_free(math_vm,"_ret");
	}
	if (v=v_find(math_vm,"_sp"))
	{
		int i;
		char *name;
		if (!(v=var_alloc(*vl,v->name,v->mode,&(v->v)))) goto err;
		*vl=v;
		v_free(math_vm,"_sp");
		for(i=v->v.v_int-1;i>=0;i--)
		{
			name=spget("_sp_",i);
			if (v=v_find(math_vm,name))
			{
				if (!(v=var_alloc(*vl,v->name,v->mode,&(v->v)))) goto err;
				*vl=v;
				v_free(math_vm,name);
			}
		}
	}
	return 0;
	err:
	v_list(math_vm,*vl);
	var_listfree(*vl);
	*vl=NULL;
	return -1;
}

void math_run_free(void)
{
	var *v;
	if (v=v_find(math_vm,"_start")) v_free(math_vm,"_start");
	if (v=v_find(math_vm,"_ret")) v_free(math_vm,"_ret");
	if (v=v_find(math_vm,"_sp"))
	{
		int i;
		char *name;
		for(i=v->v.v_int-1;i>=0;i--)
		{
			name=spget("_sp_",i);
			if (v=v_find(math_vm,name)) v_free(math_vm,name);
		}
		v_free(math_vm,"_sp");
	}
}

int math_run_init(void *_start, int **__ret)
{
	value vp;
	var *v;
	vp.v_void=_start;
	if (v=v_find(math_vm,"_start"))
	{
		value_free(v);
		v->mode=m_gen(type_void,auth_noset|auth_norev,leng_no);
		v->v.v_void=vp.v_void;
	}
	else if (!v_alloc(math_vm,"_start",m_gen(type_void,auth_noset|auth_norev,leng_no),&vp)) goto err;
	vp.v_int=0;
	if (v=v_find(math_vm,"_ret"))
	{
		value_free(v);
		v->mode=m_gen(type_int,auth_noset|auth_enrev,leng_no);
		v->v.v_void=vp.v_void;
	}
	else if (!(v=v_alloc(math_vm,"_ret",m_gen(type_int,auth_noset|auth_enrev,leng_no),&vp))) goto err;
	*__ret=&(v->v.v_int);
	if (v=v_find(math_vm,"_sp"))
	{
		value_free(v);
		v->mode=m_gen(type_int,auth_noset|auth_norev,leng_no);
		v->v.v_void=vp.v_void;
	}
	else if (!v_alloc(math_vm,"_sp",m_gen(type_int,auth_noset|auth_norev,leng_no),&vp)) goto err;
	return 0;
	err:
	*__ret=NULL;
	return 1;
}

int math_run(char *exp, char *lab, int is_vmres, var *vlist)
{
	varmat *_vm;
	var *_vl;
	int *__ret,_ret;
	var vp,*vl;
	_vm=math_vm;
	_vl=NULL;
	// is_vmres :0 = use math_vm + vl, :1 = use copy(math_vm) + vl, :2 = use vl
	if (is_vmres==1)
	{
		math_vm=v_list(NULL,vlist);
		v_add(math_vm,_vm);
		var_listfree(_vl);
		_vl=NULL;
	}
	else if (is_vmres==2) math_vm=v_list(NULL,vlist);
	else math_vm=v_list(math_vm,vlist);
	if (!math_vm) goto err;
	// 生成系统变量
	if (math_run_save(&_vl)) goto err;
	if (math_run_init(exp,&__ret)) goto err;
	// 查找标签
	if (lab)
	{
		do {
			if (*exp=='#')
			{
				if (scmp(exp+1,lab)) break;
			}
			while(*exp&&*exp!='\n') exp++;
			if (*exp) exp++;
		}
		while(*exp);
	}
	// 开始执行
	while(*exp)
	{
		while(is_space(*exp)||*exp==';') exp++;
		switch(*exp)
		{
			case '#':
				while(*exp&&*exp!='\n') exp++;
				break;
			case ':':
				exp=cal(exp,&vp);
				vl=&vp;
				goto _check_vp;
			case '.':
				goto _get_vp;
			default:
				if (!is_Name(*exp))
				{
					if (*exp==0) goto end;
					dp("math_run: 非法字符 \'%c\'(%d)\n",*exp,*exp);
					goto err;
				}
				_get_vp:
				exp=get_var(exp,&vl,&vp);
				while(is_space(*exp)||*exp==';') exp++;
				_check_vp:
				switch(m_type(vl->mode))
				{
					case type_err:
						goto err;
					case type_end:
						*__ret=vl->v.v_int;
						goto end;
					case type_int:
						*__ret=vl->v.v_int;
						break;
					case type_long:
						*__ret=vl->v.v_long;
						break;
					case type_string:
					case type_object:
						*__ret=(vl->v.v_void)?1:0;
						break;
				}
				break;
		}
	}
	goto end;
	err:
	_ret=1;
	if (*exp==0) exp--;
	while(is_space(*exp)||*exp==';') exp--;
	vl=v_find(math_vm,"_start");
	if (vl&&vl->v.v_string)
	{
		lab=vl->v.v_string;
		for(;exp>=lab;exp--)
		{
			if (*exp=='\n') _ret++;
		}
	}
	dp("math_run: 发现一些错误，运行终止于 %d 行\n",_ret);
	*__ret=-1;
	end:
	_ret=*__ret;
	if (is_vmres==1||is_vmres==2) varmat_free(math_vm);
	math_vm=_vm;
	if (_vl)
	{
		math_run_free();
		v_list(math_vm,_vl);
	}
	return _ret;
}

