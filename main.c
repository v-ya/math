#include "math.h"


char* get_exp(char *path)
{
	FILE *fp=fopen(path,"r");
	int size;
	char* exp;
	
	if (!fp) goto err;
	fseek(fp,0,2);
	size=ftell(fp);
	exp=malloc(size+1);
	if (!exp) goto err;
	fseek(fp,0,0);
	for(int i=0;i<size;i++)
	{
		exp[i]=getc(fp);
	}
	exp[size]=0;
	fclose(fp);
	return exp;
	err:
	if (fp) fclose(fp);
	return NULL;
}

char *str=NULL,*path=NULL,*debug=NULL,*lab=NULL;
var *vl=NULL;
int ret;
#define	command_size 4096
char command[command_size];

void put_command(void)
{
	printf("ret = %d : ",ret);
}

int get_command(void)
{
	int c,n=0;
	while(n<(command_size-1))
	{
		c=getchar();
		if (c==27)
		{
			command[0]=0;
			return 1;
		}
		else if (c=='\b')
		{
			if (n>0) n--;
		}
		else if (c=='\n')
		{
			if (n>0&&command[n-1]=='`')
			{
				printf("> ");
				command[n-1]='\n';
			}
			else
			{
				command[n]=0;
				break;
			}
		}
		else command[n++]=c;
	}
	if (command[0]=='q'&&command[1]=='u'&&command[2]=='i'&&command[3]=='t'&&command[4]==0) return 0;
	return 1;
}

int main(int argc, char *argv[])
{
	int n;
	for(n=1;n<argc;n++)
	{
		if (argv[n][0]=='-')
		{
			if (strcmp(argv[n],"-h")==0||strcmp(argv[n],"--help")==0)
			{
				printf(
					"math [-h | --help]\n"
					"math [-debug <filepath>] [-start <label>] <math_script>\n"
					"-h, --help\t:print help\n"
					"-debug\t\t:print stdout to <filepath>\n"
					"-start\t\t:jump <label> and begin\n"
					"# Detailed description:\n"
					"var type:\n"
					"\t:void, void[]\n"
					"\t:int, int[]\n"
					"\t:long, long[]\n"
					"\t:float, float[]\n"
					"\t:string, string[]\n"
					"\t:object\n"
					"set and unset var:\n"
					"\t:[object].set(\"type1:name1\", \"type2:name2[size2]\", ...);\n"
					"\t:[object].unset(\"name1\", \"name2\", ...);\n"
					"calculate expression:\n"
					"\t:begin at : and end at ;\n"
					"\t:Operator: :,(,),+,-,*,/,=,>,<.&,|,!,@\n"
					"define label:\n"
					"\t:The #label at start of one line will become label or note\n"
					".d.pg, .d.pv, .d.po:\n"
					"\t.d.pg(); will print global var, like system value or function\n"
					"\t.d.pv(); will print generate var\n"
					"\t.d.po(object1, object2, ...); will print child of object\n"
					"example:\n"
					"\t#start\n"
					"\t.set(\"int:i\",\"int:n\");\n"
					"\t:0@i@n;\n"
					"\t# This is a loop to calculate 1+2+...+100\n"
					"\t#loop\n"
					"\t:i+1@i\n"
					"\t:n+i@n;\n"
					"\t.if(:i<100); .jup(\"loop\");\n"
					"\t.echo(\"1+2+...+100=%%d\\;\\n\",n);\n"
					"\t.exit(0);\n"
					"math v1.0.2 from vya\n"
				);
				return 0;
			}
			else if (strcmp(argv[n],"-debug")==0)
			{
				n++;
				if (n<argc) debug=argv[n];
			}
			else if (strcmp(argv[n],"-start")==0)
			{
				n++;
				if (n<argc) lab=argv[n];
			}
			else if (strcmp(argv[n],"-var")==0)
			{
				
			}
		}
		else
		{
			path=argv[n];
			n++;
			break;
		}
	}
	_debug_init(debug);
	if (path)
	{
		str=get_exp(path);
		if (!str)
		{
			dp("不能打开 %s\n",path);
			return -1;
		}
		glob_init();
		return math_run(str,lab,0,vl);
	}
	else
	{
		printf("quit<enter> will quit\n");
		glob_init();
		math_vm=v_list(NULL,vl);
		put_command();
		while(get_command())
		{
			ret=math_run(command,NULL,0,0);
			put_command();
		}
		varmat_free(math_vm);
		varmat_free(glob_vm);
		return ret;
	}
}

