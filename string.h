#ifndef	_string_h_
#define	_string_h_

#define is_Name(c)	((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c=='_'))
#define is_name(c)	((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c=='_')||(c>='0'&&c<='9'))

#define is_space(c)	(c==' '||c=='\n'||c=='\t'||c=='\r')

#ifndef __nosym__

// 0 : ==, 1 : !=
int scmp(char *, char *);
char* sskip(char *);
char* sget(char *);
int scmp_skip(char **, char *);
char* spget(char *, int);
char* str_skip(char *);
char* str_alloc(char **);

#endif

#endif

