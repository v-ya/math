#ifndef	_calculus_h_
#define	_calculus_h_

double *_acc;
double *_inn;
int *_solv_in;
double *_solv_ac;

double _calc_diff(char *exp, var *vp)
{
	var v;
	double px,r;
	volatile double x1,x2;
	r=vp->v.v_float;
	if (fabs(r)<1) px=*_acc;
	else px=r*(*_acc);
	x1=r+px;
	x2=r-px;
	vp->v.v_float=x1;
	cal(exp,&v);
	r=v.v.v_float;
	vp->v.v_float=x2;
	cal(exp,&v);
	r-=v.v.v_float;
	r/=(x1-x2);
	return r;
}

double _calc_diffq(char *exp, var *vp)
{
	var v;
	double px,r;
	volatile double x1,x2,x3,x4;
	r=vp->v.v_float;
	if (fabs(r)<1) px=*_acc;
	else px=r*(*_acc);
	x1=r+2*px;
	x2=r+px;
	x3=r-px;
	x4=r-2*px;
	vp->v.v_float=x1;
	cal(exp,&v);
	x1=v.v.v_float;
	vp->v.v_float=x2;
	cal(exp,&v);
	x2=v.v.v_float;
	vp->v.v_float=x3;
	cal(exp,&v);
	x3=v.v.v_float;
	vp->v.v_float=x4;
	cal(exp,&v);
	x4=v.v.v_float;
	r=8*x2-8*x3+x4-x1;
	r/=12*px;
	return r;
}

char* _calc_diffs_get(char **sp, int *err)
{
	char *s=*sp;
	int n;
	while(*s&&*s!=';')
	{
		if (is_Name(*s)||*s=='.')
		{
			char *s1,*s2;
			*sp=s;
			n=0;
			while(1)
			{
				n+=strlen(sget(s));
				s=sskip(s);
				while(is_space(*s)) s++;
				if (*s=='.')
				{
					s++;
					n++;
				}
				while(is_space(*s)) s++;
				if (is_Name(*s)) ;
				else if (*s=='(')
				{
					int ns=1;
					n++;
					s++;
					while(ns&&*s&&*s!=';')
					{
						if (*s=='(') ns++;
						else if (*s==')') ns--;
						n++;
						s++;
					}
					break;
				}
				else break;
			}
			n++;
			s1=malloc(n);
			if (!s1) goto _err;
			s=*sp;
			n=0;
			while(1)
			{
				s2=sget(s);
				strcpy(s1+n,s2);
				n+=strlen(s2);
				s=sskip(s);
				while(is_space(*s)) s++;
				if (*s=='.')
				{
					s++;
					s1[n++]='.';
				}
				while(is_space(*s)) s++;
				if (is_Name(*s)) ;
				else if (*s=='(')
				{
					int ns=1;
					s1[n++]=*s;
					s++;
					while(ns&&*s&&*s!=';')
					{
						if (*s=='(') ns++;
						else if (*s==')') ns--;
						s1[n++]=*(s++);
					}
					break;
				}
				else break;
			}
			s1[n]=0;
			while(is_space(*s)) s++;
			*sp=s;
			*err=1;
			return s1;
		}
		else if (*s=='('||*s==',')
		{
			int ns=1;
			char *s1;
			*sp=s;
			n=1;
			s++;
			while(ns&&*s&&*s!=';')
			{
				if (*s=='(') ns++;
				else if (*s==')'||(*s==','&&ns==1)) ns--;
				n++;
				s++;
			}
			n--;
			s1=malloc(n);
			if (!s1) goto _err;
			s=*sp;
			n=0;
			ns=1;
			s++;
			while(ns&&*s&&*s!=';')
			{
				if (*s=='(') ns++;
				else if (*s==')'||(*s==','&&ns==1)) ns--;
				if (*s==','&&ns==0) s1[n++]=')';
				else s1[n++]=*(s++);
			}
			s1[--n]=0;
			while(is_space(*s)) s++;
			*sp=s;
			return s1;
		}
		else if ((*s>='0'&&*s<='9')||(*s=='-'&&s[1]>='0'&&s[1]<='9'))
		{
			char *s1;
			*sp=s;
			if (*s=='-')
			{
				n=1;
				s++;
			}
			else n=0;
			while(*s>='0'&&*s<='9')
			{
				n++;
				s++;
			}
			if (*s!='.') goto _step1;
			n++;
			s++;
			while(*s>='0'&&*s<='9')
			{
				n++;
				s++;
			}
			_step1:
			n++;
			s1=malloc(n);
			if (!s1) goto _err;
			s=*sp;
			n=0;
			if (*s=='-') s1[n++]=*(s++);
			while(*s>='0'&&*s<='9') s1[n++]=*(s++);
			if (*s!='.') goto _step2;
			s1[n++]=*(s++);
			while(*s>='0'&&*s<='9') s1[n++]=*(s++);
			_step2:
			s1[n]=0;
			while(is_space(*s)) s++;
			*sp=s;
			*err=1;
			return s1;
		}
		else if (*s=='+'||*s=='-'||*s=='*'||*s=='/')
		{
			*sp=s;
			s=malloc(2);
			if (!s) goto _err;
			s[0]='0';
			s[1]=0;
			return s;
		}
		s++;
	}
	*sp=s;
	return NULL;
	_err:
	*err=-1;
	return NULL;
}

static char *x;

#define	fun_num 19

static int fun_argc[fun_num]={
	1,
	1,
	2,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1
};

static char *fun_name[fun_num]={
	"sqrt",
	"squa",
	"pow",
	"exp",
	"ln",
	"lg",
	"log",
	"sin",
	"cos",
	"tan",
	"asin",
	"acos",
	"atan",
	"sinh",
	"cosh",
	"tanh",
	"asinh",
	"acosh",
	"atanh"
};

static char *fun_diffs[fun_num]={
	"(#1/.sqrt($1)/2)",
	"(#1*2*($1))",
	"((#1*($2)/($1)+(#2*.ln($1)))*.pow($1,$2))",
	"(#1*.exp($1))",
	"(#1/($1))",
	"(#1/($1)/.const.ln2)",
	"(#1/($1)/.const.ln10)",
	"(#1*.cos($1))",
	"(-#1*.sin($1))",
	"(#1/.squa(.cos($1)))",
	"(#1/.sqrt(1-.squa($1)))",
	"(-#1/.sqrt(1-.squa($1)))",
	"(#1/(1+.squa($1)))",
	"(#1*.cosh($1))",
	"(#1*.sinh($1))",
	"(1/.squa(.cosh($1)))",
	"(#1/.sqrt(.squa($1)+1))",
	"(#1/.sqrt(.squa($1)-1))",
	"(#1/(1-.squa($1)))"
};

char* _calc_diffs(char *s, int *err);

char* _calc_diffs_diff(char *s, int *err)
{
	char *n1,*n2;
	if (is_Name(*s)||*s=='.')
	{
		n1=s;
		n2=x;
		if (n2[0]=='.') goto _step1;
		while(1)
		{
			if (!scmp(n1,n2)) break;
			n1=sskip(n1);
			n2=sskip(n2);
			_step1:
			if (*n2==0)
			{
				// 1
				n1=malloc(2);
				if (!n1)
				{
					*err=-1;
					return NULL;
				}
				n1[0]='1';
				n1[1]=0;
				return n1;
			}
			else if (*n1!='.'||*n2!='.') break;
			n1++;
			n2++;
		}
		n1=s;
		while(*n1&&*n1!='(') n1++;
		if (*n1=='(')
		{
			// func
			char *x1=NULL,*x2=NULL,*y1=NULL,*y2=NULL;
			int fn=0;
			if (*s=='.') s++;
			while(!scmp(s,fun_name[fn]))
			{
				fn++;
				if (fn>=fun_num) goto err_func;
			}
			if (fun_argc[fn]==1)
			{
				x1=_calc_diffs_get(&n1,err);
				if (*err<0) goto err_func;
			}
			else if (fun_argc[fn]==2)
			{
				x1=_calc_diffs_get(&n1,err);
				if (*err<0) goto err_func;
				x2=_calc_diffs_get(&n1,err);
				if (*err<0) goto err_func;
			}
			else goto err_func;
			if (x1)
			{
				y1=_calc_diffs(x1,err);
				if (*err<0) goto err_func;
			}
			if (x2)
			{
				y2=_calc_diffs(x2,err);
				if (*err<0) goto err_func;
			}
			n2=fun_diffs[fn];
			fn=0;
			n1=n2;
			while(*n1)
			{
				if (*n1=='$')
				{
					n1++;
					if (*n1=='1'&&x1) fn+=strlen(x1);
					else if (*n1=='2'&&x2) fn+=strlen(x2);
				}
				else if (*n1=='#')
				{
					n1++;
					if (*n1=='1'&&y1) fn+=strlen(y1);
					else if (*n1=='2'&&y2) fn+=strlen(y2);
				}
				else fn++;
				n1++;
			}
			fn++;
			n1=malloc(fn);
			if (!n1) goto err_func;
			fn=0;
			while(*n2)
			{
				if (*n2=='$')
				{
					n2++;
					n1[fn]=0;
					if (*n2=='1'&&x1) strcpy(n1+fn,x1);
					else if (*n2=='2'&&x2) strcpy(n1+fn,x2);
					while(n1[fn]) fn++;
					n2++;
				}
				else if (*n2=='#')
				{
					n2++;
					n1[fn]=0;
					if (*n2=='1'&&y1) strcpy(n1+fn,y1);
					else if (*n2=='2'&&y2) strcpy(n1+fn,y2);
					while(n1[fn]) fn++;
					n2++;
				}
				else n1[fn++]=*(n2++);
			}
			n1[fn]=0;
			if (x1) free(x1);
			if (x2) free(x2);
			if (y1) free(y1);
			if (y2) free(y2);
			return n1;
			err_func:
			if (x1) free(x1);
			if (x2) free(x2);
			if (y1) free(y1);
			if (y2) free(y2);
			*err=-1;
			return NULL;	
		}
	}
	// 0
	n1=malloc(2);
	if (!n1)
	{
		*err=-1;
		return NULL;
	}
	n1[0]='0';
	n1[1]=0;
	return n1;
}

int _calc_diffs_isfloat(char *s)
{
	while(*s=='('||*s==')') s++;
	if (*s=='-') s++;
	while(*s>='0'&&*s<='9') s++;
	if (*s=='.') s++;
	while(*s>='0'&&*s<='9') s++;
	while(*s=='('||*s==')') s++;
	if (*s==0) return 1;
	else return 0;
}

double _calc_diffs_float(char *s)
{
	
	while(*s=='('||*s==')') s++;
	return atof(s);
}

char* _calc_diffs_sfloat(double f, int *err)
{
	double r,p;
	int n;
	char *s;
	n=1;
	r=f;
	if (r<0)
	{
		n+=3;
		r=fabs(r);
	}
	p=1;
	while(p<=r) p*=10;
	if (p<5) n++;
	while(p>5)
	{
		p/=10;
		n++;
		r=fmod(r,p);
	}
	if (r) n++;
	while(r!=0.0)
	{
		p/=10;
		n++;
		r=fmod(r,p);
	}
	s=malloc(n);
	if (!s)
	{
		*err=-1;
		return NULL;
	}
	r=f;
	n=0;
	if (r<0)
	{
		s[n++]='(';
		s[n++]='-';
		r=fabs(r);
	}
	p=1;
	while(p<=r) p*=10;
	if (p<5) s[n++]='0';
	while(p>5)
	{
		p/=10;
		s[n++]=(int)(r/p)+'0';
		r=fmod(r,p);
	}
	if (r) s[n++]='.';
	while(r!=0.0)
	{
		p/=10;
		s[n++]=(int)(r/p)+'0';
		r=fmod(r,p);
	}
	if (f<0) s[n++]=')';
	s[n]=0;
	return s;
}

char* _calc_diffs_restr(char *s, int *err)
{
	char *s1,*s2;
	int n,ns;
	s1=s;
	n=strlen(s);
	s2=s;
	n++;
	
	while(1)
	{
		while(*s1&&*s1!='(') s1++;
		if (*s1==0) break;
		if (s1[1]=='(')
		{
			s2=s1+2;
			ns=2;
			while(ns)
			{
				if (*s2=='(') ns++;
				else if (*s2==')') ns--;
				s2++;
				if (ns==1&&s2[0]==')'&&s2[1]==')')
				{
					*s1=' ';
					*(s2+1)=' ';
					n-=2;
					break;
				}
			}
		}
		s1++;
	}
	s1=malloc(n);
	if (!s1)
	{
		*err=-1;
		return NULL;
	}
	n=0;
	while(*s)
	{
		if (*s==' ') s++;
		else s1[n++]=*(s++);
	}
	s1[n]=0;
	return s1;
}

char* _calc_diffs(char *s, int *err)
{
	#define	_is_0(S) (S&&S[0]=='0'&&S[1]==0)
	#define	_is_1(S) (S&&S[0]=='1'&&S[1]==0)
	char *s1=NULL,*y1=NULL,*s2=NULL,*y2=NULL,*sl;
	int c;
	s1=_calc_diffs_get(&s,err);
	if (*err==-1) goto err;
	else if (!s1) goto err;
	else if (*err==1)
	{
		y1=_calc_diffs_diff(s1,err);
		if (*err==-1) goto err;
		*err=0;
	}
	else
	{
		y1=_calc_diffs(s1,err);
		if (*err==-1) goto err;
	}
	while(is_space(*s)) s++;
	while(*s&&*s!=';')
	{
		c=*(s++);
		if (c!='+'&&c!='-'&&c!='*'&&c!='/') goto next;
		s2=_calc_diffs_get(&s,err);
		if (*err==-1) goto err;
		else if (!s2) goto err;
		else if (*err==1)
		{
			y2=_calc_diffs_diff(s2,err);
			if (*err==-1) goto err;
			*err=0;
		}
		else
		{
			y2=_calc_diffs(s2,err);
			if (*err==-1) goto err;
		}
		switch (c)
		{
			case '+':
				if (_is_0(s1))
				{
					free(s1);
					s1=s2;
					s2=NULL;
				}
				else if (_is_0(s2))
				{
					free(s2);
					s2=NULL;
				}
				else if (_calc_diffs_isfloat(s1)&&_calc_diffs_isfloat(s2))
				{
					sl=_calc_diffs_sfloat(_calc_diffs_float(s1)+_calc_diffs_float(s2),err);
					if (*err==-1) goto err;
					free(s1);
					free(s2);
					s2=NULL;
					s1=sl;
				}
				else
				{
					sl=malloc(6+strlen(s1)+strlen(s2));
					if (!sl) goto err;
					sprintf(sl,"(%s)+(%s)",s1,s2);
					free(s1);
					free(s2);
					s2=NULL;
					s1=sl;
				}
				if (_is_0(y1))
				{
					free(y1);
					y1=y2;
					y2=NULL;
				}
				else if (_is_0(y2))
				{
					free(y2);
					y2=NULL;
				}
				else if (_calc_diffs_isfloat(y1)&&_calc_diffs_isfloat(y2))
				{
					sl=_calc_diffs_sfloat(_calc_diffs_float(y1)+_calc_diffs_float(y2),err);
					if (*err==-1) goto err;
					free(y1);
					free(y2);
					y2=NULL;
					y1=sl;
				}else
				{
					sl=malloc(4+strlen(y1)+strlen(y2));
					if (!sl) goto err;
					sprintf(sl,"(%s+%s)",y1,y2);
					free(y1);
					free(y2);
					y2=NULL;
					y1=sl;
				}
				break;
			case '-':
				if (_is_0(s1))
				{
					free(s1);
					s1=malloc(strlen(s2)+4);
					if (!s1) goto err;
					sprintf(s1,"-(%s)",s2);
					free(s2);
					s2=NULL;
				}
				else if (_is_0(s2))
				{
					free(s2);
					s2=NULL;
				}
				else if (_calc_diffs_isfloat(s1)&&_calc_diffs_isfloat(s2))
				{
					sl=_calc_diffs_sfloat(_calc_diffs_float(s1)-_calc_diffs_float(s2),err);
					if (*err==-1) goto err;
					free(s1);
					free(s2);
					s2=NULL;
					s1=sl;
				}
				else
				{
					sl=malloc(6+strlen(s1)+strlen(s2));
					if (!sl) goto err;
					sprintf(sl,"(%s)-(%s)",s1,s2);
					free(s1);
					free(s2);
					s2=NULL;
					s1=sl;
				}
				if (_is_0(y1))
				{
					free(y1);
					y1=malloc(strlen(y2)+4);
					if (!y1) goto err;
					sprintf(y1,"(-%s)",y2);
					free(y2);
					y2=NULL;
				}
				else if (_is_0(y2))
				{
					free(y2);
					y2=NULL;
				}
				else if (_calc_diffs_isfloat(y1)&&_calc_diffs_isfloat(y2))
				{
					sl=_calc_diffs_sfloat(_calc_diffs_float(y1)-_calc_diffs_float(y2),err);
					if (*err==-1) goto err;
					free(y1);
					free(y2);
					y2=NULL;
					y1=sl;
				}else
				{
					sl=malloc(4+strlen(y1)+strlen(y2));
					if (!sl) goto err;
					sprintf(sl,"(%s-%s)",y1,y2);
					free(y1);
					free(y2);
					y2=NULL;
					y1=sl;
				}
				break;
			case '*':
				if (_is_0(y1)) ;
				else if (_is_0(s2))
				{
					free(y1);
					y1=malloc(strlen(s2)+1);
					if (!y1) goto err;
					strcpy(y1,s2);
				}
				else if (_is_1(y1))
				{
					free(y1);
					y1=malloc(strlen(s2)+3);
					if (!y1) goto err;
					sprintf(y1,"(%s)",s2);
				}
				else if (_is_1(s2)) ;
				else if (_calc_diffs_isfloat(y1)&&_calc_diffs_isfloat(s2))
				{
					sl=_calc_diffs_sfloat(_calc_diffs_float(y1)*_calc_diffs_float(s2),err);
					if (*err==-1) goto err;
					free(y1);
					y1=sl;
				}else
				{
					sl=malloc(6+strlen(y1)+strlen(s2));
					if (!sl) goto err;
					sprintf(sl,"(%s*(%s))",y1,s2);
					free(y1);
					y1=sl;
				}
				if (_is_0(y2)) ;
				else if (_is_0(s1))
				{
					free(y2);
					y2=malloc(strlen(s1)+1);
					if (!y2) goto err;
					strcpy(y2,s1);
				}
				else if (_is_1(y2))
				{
					free(y2);
					y2=malloc(strlen(s1)+3);
					if (!y2) goto err;
					sprintf(y2,"(%s)",s1);
				}
				else if (_is_1(s1)) ;
				else if (_calc_diffs_isfloat(y2)&&_calc_diffs_isfloat(s1))
				{
					sl=_calc_diffs_sfloat(_calc_diffs_float(y2)*_calc_diffs_float(s1),err);
					if (*err==-1) goto err;
					free(y2);
					y2=sl;
				}else
				{
					sl=malloc(6+strlen(y2)+strlen(s1));
					if (!sl) goto err;
					sprintf(sl,"(%s*(%s))",y2,s1);
					free(y2);
					y2=sl;
				}
				if (_is_0(y1))
				{
					free(y1);
					y1=y2;
					y2=NULL;
				}
				else if (_is_0(y2))
				{
					free(y2);
					y2=NULL;
				}
				else if (_calc_diffs_isfloat(y1)&&_calc_diffs_isfloat(y2))
				{
					sl=_calc_diffs_sfloat(_calc_diffs_float(y1)+_calc_diffs_float(y2),err);
					if (*err==-1) goto err;
					free(y1);
					free(y2);
					y2=NULL;
					y1=sl;
				}else
				{
					sl=malloc(4+strlen(y1)+strlen(y2));
					if (!sl) goto err;
					sprintf(sl,"(%s+%s)",y1,y2);
					free(y1);
					free(y2);
					y2=NULL;
					y1=sl;
				}
				if (_is_0(s1))
				{
					free(s2);
					s2=NULL;
				}
				else if (_is_0(s2))
				{
					free(s1);
					s1=s2;
					s2=NULL;
				}
				else if (_is_1(s1))
				{
					free(s1);
					s1=s2;
					s2=NULL;
				}
				else if (_is_1(s2))
				{
					free(s2);
					s2=NULL;
				}
				else if (_calc_diffs_isfloat(s1)&&_calc_diffs_isfloat(s2))
				{
					sl=_calc_diffs_sfloat(_calc_diffs_float(s1)*_calc_diffs_float(s2),err);
					if (*err==-1) goto err;
					free(s1);
					free(s2);
					s2=NULL;
					s1=sl;
				}
				else
				{
					sl=malloc(6+strlen(s1)+strlen(s2));
					if (!sl) goto err;
					sprintf(sl,"(%s)*(%s)",s1,s2);
					free(s1);
					free(s2);
					s2=NULL;
					s1=sl;
				}
				break;
			case '/':
				if (_is_0(y1)) ;
				else if (_is_1(s2)) ;
				else if (_calc_diffs_isfloat(y1)&&_calc_diffs_isfloat(s2))
				{
					sl=_calc_diffs_sfloat(_calc_diffs_float(y1)/_calc_diffs_float(s2),err);
					if (*err==-1) goto err;
					free(y1);
					y1=sl;
				}else
				{
					sl=malloc(6+strlen(y1)+strlen(s2));
					if (!sl) goto err;
					sprintf(sl,"(%s/(%s))",y1,s2);
					free(y1);
					y1=sl;
				}
				if (_is_0(s1))
				{
					free(y2);
					y2=malloc(strlen(s1)+1);
					if (!y2) goto err;
					strcpy(y2,s1);
				}
				else if (_is_1(s1)) ;
				else if (_is_1(y2))
				{
					free(y2);
					y2=malloc(strlen(s1)+6);
					if (!y2) goto err;
					sprintf(y2,"(-(%s))",s1);
				}
				else if (_calc_diffs_isfloat(y2)&&_calc_diffs_isfloat(s1))
				{
					sl=_calc_diffs_sfloat((-1/_calc_diffs_float(y2))*_calc_diffs_float(s1),err);
					if (*err==-1) goto err;
					free(y2);
					y2=sl;
				}else
				{
					sl=malloc(11+strlen(y2)+strlen(s1));
					if (!sl) goto err;
					sprintf(sl,"((-1/%s)*(%s))",y2,s1);
					free(y2);
					y2=sl;
				}
				if (_is_0(y1))
				{
					free(y1);
					y1=y2;
					y2=NULL;
				}
				else if (_is_0(y2))
				{
					free(y2);
					y2=NULL;
				}
				else if (_calc_diffs_isfloat(y1)&&_calc_diffs_isfloat(y2))
				{
					sl=_calc_diffs_sfloat(_calc_diffs_float(y1)+_calc_diffs_float(y2),err);
					if (*err==-1) goto err;
					free(y1);
					free(y2);
					y2=NULL;
					y1=sl;
				}else
				{
					sl=malloc(4+strlen(y1)+strlen(y2));
					if (!sl) goto err;
					sprintf(sl,"(%s+%s)",y1,y2);
					free(y1);
					free(y2);
					y2=NULL;
					y1=sl;
				}
				if (_is_0(s1))
				{
					free(s2);
					s2=NULL;
				}
				else if (_is_1(s2))
				{
					free(s2);
					s2=NULL;
				}
				if (_calc_diffs_isfloat(s1)&&_calc_diffs_isfloat(s2))
				{
					sl=_calc_diffs_sfloat(_calc_diffs_float(s1)/_calc_diffs_float(s2),err);
					if (*err==-1) goto err;
					free(s1);
					free(s2);
					s2=NULL;
					s1=sl;
				}
				else
				{
					sl=malloc(6+strlen(s1)+strlen(s2));
					if (!sl) goto err;
					sprintf(sl,"(%s)/(%s)",s1,s2);
					free(s1);
					free(s2);
					s2=NULL;
					s1=sl;
				}
				break;
		}
		next:
		while(is_space(*s)) s++;
	}
	sl=y1;
	y1=NULL;
	if (y2) free(y2);
	if (s1) free(s1);
	if (s2) free(s2);
	return sl;
	err:
	*err=-1;
	if (y1) free(y1);
	if (y2) free(y2);
	if (s1) free(s1);
	if (s2) free(s2);
	return NULL;
}

double _calc_inte(char *exp, var *vp, double fe)
{
	var v;
	double px,r,s;
	r=vp->v.v_float;
	px=(fe-r)*(*_inn)/2;
	if (px==0.0) px=*_inn;
	r+=px;
	px*=2;
	if (px>0) for(s=0;r<fe;r+=px)
	{
		vp->v.v_float=r;
		cal(exp,&v);
		s+=v.v.v_float*px;
	}
	else for(s=0;r>fe;r+=px)
	{
		vp->v.v_float=r;
		cal(exp,&v);
		s+=v.v.v_float*px;
	}
	return s;
}

double _calc_inteq(char *exp, var *vp, double fe)
{
	var v;
	double px,r,s;
	double fl,fr;
	r=vp->v.v_float;
	px=(fe-r)*(*_inn)/2;
	if (px==0.0) px=*_inn;
	r+=px;
	vp->v.v_float=r-px;
	cal(exp,&v);
	fr=v.v.v_float;
	if (px>0) for(s=0;r<fe;r+=px*2)
	{
		fl=fr;
		vp->v.v_float=r+px;
		cal(exp,&v);
		fr=v.v.v_float;
		vp->v.v_float=r;
		cal(exp,&v);
		s+=(fl+fr+v.v.v_float*4)*px/3;
	}
	else for(s=0;r>fe;r+=px*2)
	{
		fl=fr;
		vp->v.v_float=r+px;
		cal(exp,&v);
		fr=v.v.v_float;
		vp->v.v_float=r;
		cal(exp,&v);
		s+=(fl+fr+v.v.v_float*4)*px/3;
	}
	return s;
}

double _calc_solv(char *exp, var *vp)
{
	var v;
	int i;
	double y,k,x;
	cal(exp,&v);
	x=vp->v.v_float;
	y=v.v.v_float;
	for(i=0;(i<*_solv_in)&&(fabs(y)>*_solv_ac);i++)
	{
		vp->v.v_float=x;
		k=_calc_diff(exp,vp);
		x-=y/k;
		vp->v.v_float=x;
		cal(exp,&v);
		y=v.v.v_float;
	}
	return vp->v.v_float;
}

#endif

