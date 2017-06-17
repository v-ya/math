#include "math.h"

varmat *varmat_alloc(void)
{
	varmat *vm;
	vm=malloc(sizeof(varmat));
	if (vm) memset(vm,0,sizeof(varmat));
	else
	{
		dp("varmat_alloc: 分配空间失败\n");
		return NULL;
	}
	return vm;
}

void varmat_free(varmat *vm)
{
	int i;
	if (!vm) return ;
	for(i=0;i<256;i++)
	if (vm->vl[i]) var_listfree(vm->vl[i]);
	free(vm);
}

int varmat_ngen(char *name)
{
	int n=0;
	while(*name)
	n+=*(name++);
	return (n&0xff);
}

void value_free(var *v)
{
	int i;
	if (m_leng(v->mode)&&m_free(v->mode))
	{
		if (m_type(v->mode)==type_string)
		{
			for(i=m_leng(v->mode)-1;i>=0;i--)
			free(((char **)(v->v.v_void))[i]);
		}
		else if (m_type(v->mode)==type_object)
		{
			for(i=m_leng(v->mode)-1;i>=0;i--)
			var_listfree(((var **)(v->v.v_void))[i]);
		}
		free(v->v.v_void);
	}
	else if (m_type(v->mode)==type_void&&m_free(v->mode)) free(v->v.v_void);
	else if (m_type(v->mode)==type_string&&m_free(v->mode)) free(v->v.v_string);
	else if (m_type(v->mode)==type_object&&m_free(v->mode)) var_listfree(v->v.v_object);
	v->v.v_void=NULL;
	v->mode&=~free_need;
}

var* var_alloc(var *vl, char *name, unsigned int mode, value *vp)
{
	var *v;
	int i,turn;
	v=malloc(sizeof(var));
	if (!v)
	{
		err_alloc:
		dp("var_alloc: 变量 %s 分配空间失败\n",name);
		return NULL;
	}
	for(i=0;i<11;i++)
	{
		if (name[i]==0) break;
		v->name[i]=name[i];
	}
	v->name[i]=0;
	v->mode=mode;
	if (vp) v->v.v_void=vp->v_void;
	else v->v.v_void=NULL;
	if (!vl)
	{
		v->l=NULL;
		v->r=NULL;
		goto end;
	}
	i=0;
	turn=1;
	while(1)
	{
		for(i=0;1;i++)
		{
			if (vl->name[i]<name[i]) goto _var_name_a;
			else if (vl->name[i]>name[i]) goto _var_name_b;
			if (name[i]==0) goto _var_name_e;
		}
		_var_name_a:
		if (vl->r&&turn) vl=vl->r;
		else
		{
			v->l=vl;
			v->r=vl->r;
			if (vl->r) vl->r->l=v;
			vl->r=v;
			break;
		}
		turn=1;
		continue;
		_var_name_b:
		if (vl->l) vl=vl->l;
		else
		{
			v->r=vl;
			v->l=vl->l;
			if (vl->l) vl->l->r=v;
			vl->l=v;
			break;
		}
		turn=0;
		continue;
		_var_name_e:
		dp("var_alloc: 重名变量 %s 申请失败\n",name);
		free(v);
		return NULL;
	}
	end:
	if (m_leng(v->mode))
	{
		int size;
		void *data;
		switch (m_type(v->mode))
		{
			case type_int:
				size=sizeof(int);
				break;
			case type_long:
				size=sizeof(long long);
				break;
			case type_float:
				size=sizeof(double);
				break;
			case type_string:
				size=sizeof(char *);
				break;
			default:
				dp("var_alloc: 数组 %s 类型未定义\n",name);
				free(v);
				return NULL;
		}
		size*=m_leng(v->mode);
		data=malloc(size);
		if (data)
		{
			if ((v->v.v_void)&&m_type(v->mode)!=type_string&&m_type(v->mode)!=type_object) memcpy(data,v->v.v_void,size);
			else memset(data,0,size);
			v->v.v_void=data;
		}
		else
		{
			free(v);
			goto err_alloc;
		}
		v->mode|=free_need;
	}
	else
	{
		if (m_type(v->mode)==type_string&&v->v.v_void)
		{
			char *s;
			s=malloc(strlen(v->v.v_string)+1);
			if (s)
			{
				strcpy(s,v->v.v_string);
				v->v.v_string=s;
				v->mode|=free_need;
			}
			else
			{
				free(v);
				goto err_alloc;
			}
		}
		else if (m_type(v->mode)==type_object)
		{
			var *vl;
			if (v->v.v_void)
			{
				vl=var_listadd(NULL,v->v.v_object);
				if (vl)
				{
					v->v.v_object=vl;
					v->mode|=free_need;
				}
				else
				{
					free(v);
					goto err_alloc;
				}
			}
			else
			{
				value val;
				v->mode|=free_need;
				val.v_void=&_func_set;
				vl=var_alloc((var*)v->v.v_void,"set",
					m_gen(type_void,auth_noset|auth_norev|func_code,leng_no),&val);
				if (!vl) goto err_obj_alloc;
				v->v.v_void=vl;
				val.v_void=&_func_unset;
				vl=var_alloc((var*)v->v.v_void,"unset",
					m_gen(type_void,auth_noset|auth_norev|func_code,leng_no),&val);
				if (!vl) goto err_obj_alloc;
				v->v.v_void=vl;
				return v;
				err_obj_alloc:
				value_free(v);
				free(v);
				goto err_alloc;
			}
		}
	}
	return v;
}

var* var_ralloc(var *vl, char *name, unsigned int mode, value *vp)
{
	if (!vl) return var_alloc(vl,name,mode,vp);
	vl=var_free(vl,name);
	return var_alloc(vl,name,mode,vp);
}

var* var_find(var *vl, char *name)
{
	int i,turn=1;
	if (!vl) return NULL;
	while(1)
	{
		for(i=0;1;i++)
		{
			if (vl->name[i]<name[i]) goto _var_find_a;
			else if (vl->name[i]>name[i]) goto _var_find_b;
			if (name[i]==0) goto _var_find_e;
		}
		_var_find_a:
		if (vl->r&&turn) vl=vl->r;
		else return NULL;
		turn=1;
		continue;
		_var_find_b:
		if (vl->l) vl=vl->l;
		else return NULL;
		turn=0;
		continue;
		_var_find_e:
		return vl;
	}
}

var* var_free(var *vl, char *name)
{
	var *vp;
	vp=var_find(vl,name);
	if (vp) vl=vp;
	else return vl;
	if (m_free(vl->mode)) value_free(vl);
	if (vl->l)
	{
		vp=vl->l;
		vp->r=vl->r;
		if (vl->r) vl->r->l=vp;
	}
	else if (vl->r)
	{
		vp=vl->r;
		vp->l=NULL;
	}
	else vp=NULL;
	free(vl);
	return vp;
}

void var_listfree(var *vl)
{
	if (!vl) return ;
	while(vl->l) vl=vl->l;
	while(vl->r)
	{
		vl=vl->r;
		if (m_free(vl->l->mode)) value_free(vl->l);
		free(vl->l);
	}
	free(vl);
}

var* var_listadd(var *vl, var *vs)
{
	var *vp;
	if (!vs) return vl;
	while(vs->l) vs=vs->l;
	while(vs)
	{
		vp=var_alloc(vl,vs->name,vs->mode,&(vs->v));
		if (vp) vl=vp;
		vs=vs->r;
	}
	return vl;
}

var* v_alloc(varmat *vm, char *name, unsigned int mode, value *vp)
{
	int i;
	var *vl;
	if (!vm) return NULL;
	i=varmat_ngen(name);
	vl=var_alloc(vm->vl[i],name,mode,vp);
	if (!vl)
	{
		dp("v_alloc: var_alloc 调用失败\n");
		return NULL;
	}
	vm->vl[i]=vl;
	return vl;
}

var* v_find(varmat *vm, char *name)
{
	if (!vm) return NULL;
	return var_find(vm->vl[varmat_ngen(name)],name);
}

void v_free(varmat *vm, char *name)
{
	int i;
	if (!vm) return ;
	i=varmat_ngen(name);
	vm->vl[i]=var_free(vm->vl[i],name);
}

void v_freeall(varmat *vm)
{
	int i;
	if (!vm) return ;
	for(i=0;i<256;i++)
	if (vm->vl[i]) var_listfree(vm->vl[i]);
}

varmat* v_list(varmat *vm, var *vl)
{
	int i;
	if (!vm)
	{
		vm=varmat_alloc();
		if (!vm)
		{
			dp("v_list: varmat_alloc 调用失败\n");
			return NULL;
		}
	}
	if (!vl) return vm;
	while(vl->l) vl=vl->l;
	while(vl)
	{
		v_alloc(vm,vl->name,vl->mode,&(vl->v));
		vl=vl->r;
	}
	return vm;
}

varmat* v_add(varmat *vm, varmat *vms)
{
	int i;
	if (!vms) return vm;
	for(i=0;i<256;i++)
	vm->vl[i]=var_listadd(vm->vl[i],vms->vl[i]);
	return vm;
}

var* var_poin(var *vp, var *v)
{
	if (vp&&m_poin(vp->mode))
	{
		v->mode=m_type(vp->mode)|auth_noset;
		switch(m_type(v->mode))
		{
			case type_void:
				v->v.v_void=*((void**)(vp->v.v_void));
				break;
			case type_int:
				v->v.v_int=*((int*)(vp->v.v_void));
				break;
			case type_long:
				v->v.v_long=*((long*)(vp->v.v_void));
				break;
			case type_float:
				v->v.v_float=*((float*)(vp->v.v_void));
				break;
			case type_string:
				v->v.v_string=*((char**)(vp->v.v_void));
				break;
			case type_object:
				v->v.v_object=*((var**)(vp->v.v_void));
				break;
			default:
				v->v.v_void=NULL;
		}
		return v;
	}
	else return vp;
}

static char *_str_type[]={
	"void",
	"int",
	"long",
	"float",
	"string",
	"object"
};

char* str_type(unsigned int mode)
{
	int type,i;
	static char s[64];
	type=m_type(mode);
	if (m_func(mode)) strcpy(s,"func");
	else if (type<6) strcpy(s,_str_type[type]);
	else if (type==0xff) strcpy(s,"err");
	else strcpy(s,"?");
	if (m_leng(mode))
	{
		i=strlen(s);
		sprintf(s+i,"[%d]",m_leng(mode));
	}
	i=strlen(s);
	sprintf(s+i,"-(%s,%s,%s)",
		m_auth_set(mode)?"noset":"enset",
		m_auth_rev(mode)?"norev":"enrev",
		m_free(mode)?"free_need":"free_no"
		);
	return s;
}

void varmat_print(varmat *vms)
{
	int i,j,n,num;
	var *vl;
	varmat vm;
	num=0;
	if (!vms)
	{
		dp("未找到变量堆\n");
		return ;
	}
	for(i=0,n=0;i<256;i++)
	{
		vl=vms->vl[i];
		if (vl)
		{
			while(vl->r) vl=vl->r;
			while(vl->l)
			{
				vl=vl->l;
				num++;
			}
			num++;
			vm.vl[n++]=vl;
		}
	}
	dp("在变量堆中共找到 %d 变量\n",num);
	while(n)
	{
		j=0;
		for(i=0;i<n;i++)
		if (strcmp(vm.vl[i]->name,vm.vl[j]->name)<=0) j=i;
		vl=vm.vl[j];
		i=strlen(vl->name);
		if (i<8) dp("%s\t\t:%s",vl->name,str_type(vl->mode));
		else dp("%s\t:%s",vl->name,str_type(vl->mode));
		if (m_leng(vl->mode)==leng_no) switch (m_type(vl->mode))
		{
			case type_int:
				dp("\t%d",vl->v.v_int);
				break;
			case type_long:
				dp("\t%ld",vl->v.v_long);
				break;
			case type_float:
				dp("\t%g",vl->v.v_float);
				break;
		}
		dp("\n");
		vl=vl->r;
		if (vl) vm.vl[j]=vl;
		else
		{
			n--;
			vm.vl[j]=vm.vl[n];
			vm.vl[n]=NULL;
		}
	}
}

void varlist_print(var *vl)
{
	int num=1;
	if (!vl)
	{
		dp("未找到变量链\n");
		return ;
	}
	while(vl->r) vl=vl->r;
	while(vl->l)
	{
		vl=vl->l;
		num++;
	}
	dp("在变量链中共找到 %d 变量\n",num);
	while(vl)
	{
		if (strlen(vl->name)<8) dp("%s\t\t:%s",vl->name,str_type(vl->mode));
		else dp("%s\t:%s",vl->name,str_type(vl->mode));
		if (m_leng(vl->mode)==leng_no) switch (m_type(vl->mode))
		{
			case type_int:
				dp("\t%d",vl->v.v_int);
				break;
			case type_long:
				dp("\t%ld",vl->v.v_long);
				break;
			case type_float:
				dp("\t%g",vl->v.v_float);
				break;
		}
		dp("\n");
		vl=vl->r;
	}
}

void var_print(var *vl)
{
	
}


