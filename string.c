#include "math.h"

int scmp(char *str0, char *str1)
{
	int c;
	for(;1;str0++,str1++)
	{
		c=*str0;
		if (is_name(c))
		{
			if (*str1==c) continue;
			else return 0;
		}
		c=*str1;
		if (is_name(c)) return 0;
		else return 1;
	}
}

char* sskip(char *str)
{
	if (!is_Name(*str)) return str;
	while(is_name(*str)) str++;
	return str;
}

char* sget(char *str)
{
	static char s[32]={0};
	int i;
	if (!is_Name(*str))
	{
		s[0]=0;
		return s;
	}
	for(i=0;i<30;i++)
	{
		if (is_name(*str)) s[i]=*(str++);
		else break;
	}
	if (i<30) s[i]=0;
	else s[30]='<';
	return s;
}

int scmp_skip(char **str0,char *str1)
{
	int r;
	r=scmp(*str0,str1);
	if (r) *str0=sskip(*str0);
	return r;
}

char* spget(char *sa, int n)
{
	static char s[32]={0};
	int i,c;
	sprintf(s,"%s%d",sa,n);
	return s;
}

char* str_skip(char *s)
{
	if (*s!='\"') return s;
	s++;
	while(1)
	{
		switch (*s)
		{
			case '\"':
			case ';':
			case '\n':
			case 0:
				goto end;
			case '\\':
				s++;
				if (*s=='x')
				{
					s++;
					if ((*s>='0'&&*s<='9')&&(*s>='A'&&*s<='F')&&(*s>='a'&&*s<='f')) s++;
					if ((*s>='0'&&*s<='9')&&(*s>='A'&&*s<='F')&&(*s>='a'&&*s<='f')) s++;
				}
				else s++;
				break;
			default:
				s++;
				break;
		}
	}
	end:
	if (*s=='\"') s++;
	return s;
}

char* str_alloc(char **exp)
{
	char *s=*exp,*str,*str1=NULL;
	int n=0;
	if (*s!='\"') goto end;
	*exp=++s;
	while(1)
	{
		switch (*s)
		{
			case '\"':
			case ';':
			case '\n':
			case 0:
				goto _s_1;
			case '\\':
				s++;
				if (*s=='x')
				{
					s++;
					if ((*s>='0'&&*s<='9')&&(*s>='A'&&*s<='F')&&(*s>='a'&&*s<='f')) s++;
					if ((*s>='0'&&*s<='9')&&(*s>='A'&&*s<='F')&&(*s>='a'&&*s<='f')) s++;
				}
				else s++;
				break;
			default:
				s++;
				break;
		}
		n++;
	}
	_s_1:
	n++;
	str=malloc(n);
	str1=str;
	if (!str)
	{
		dp("str_alloc: 申请字符串空间失败\n");
		goto end;
	}
	s=*exp;
	while(1)
	{
		switch (*s)
		{
			case '\"':
			case ';':
			case '\n':
			case 0:
				*str=0;
				goto end;
			case '\\':
				s++;
				if (*s=='x')
				{
					s++;
					n=0;
					if ((*s>='0'&&*s<='9')||(*s>='A'&&*s<='F')||(*s>='a'&&*s<='f'))
					{
						n=(*s>='0'&&*s<='9')?(*s-'0'):(*s>='A'&&*s<='F')?(*s-'A'+10):(*s-'a'+10);
						s++;
					}
					if ((*s>='0'&&*s<='9')||(*s>='A'&&*s<='F')||(*s>='a'&&*s<='f'))
					{
						n=(n<<4)+((*s>='0'&&*s<='9')?(*s-'0'):(*s>='A'&&*s<='F')?(*s-'A'+10):(*s-'a'+10));
						s++;
					}
					*(str++)=n;
				}
				else
				{
					switch (*s)
					{
						case 'n':
							*(str++)='\n';
							break;
						case 't':
							*(str++)='\t';
							break;
						case 'r':
							*(str++)='\r';
							break;
						case 0:
							*(str++)='\\';
							break;
						default:
							*(str++)=*s;
							break;
					}
					s++;
				}
				break;
			default:
				*(str++)=*s;
				s++;
				break;
		}
	}
	end:
	if (*s=='\"') s++;
	*exp=s;
	return str1;
}

