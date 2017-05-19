#ifndef	_var_h_
#define	_var_h_

#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER )

struct VAR;
union VALUE;
struct VARMAT;

typedef union VALUE {
	void *v_void;
	int v_int;
	long long v_long;
	double v_float;
	char *v_string;
	struct VAR *v_object;
} value;

// type
#define	m_type(m)	((m)&0xff)
#define	type_err	0xff
#define	type_end	0xfe
#define	type_void	0
#define	type_int	1
#define	type_long	2
#define	type_float	3
#define	type_string	4
#define	type_object	5

// authority
#define m_auth_set(m)	((m)&0x0100)
#define m_auth_rev(m)	((m)&0x0200)
#define	auth_enset	0x0000
#define	auth_noset	0x0100
#define	auth_enrev	0x0000
#define auth_norev	0x0200
// free
#define m_free(m)	((m)&0x0400)
#define free_need	0x0400
#define free_not	0x0000
// func
#define m_func(m)	((m)&0x0800)
#define func_code	0x0800
#define func_not	0x0000
// pointer
#define m_poin(m)	((m)&0x1000)
#define type_pointer	0x1000
// length
#define m_leng(m)	((m)>>16)
#define leng_no		0
// get mode
#define m_gen(t,a,l)	((t)|(a)|((l)<<16))

typedef struct VAR {
	char name[12];
	unsigned int mode;
	value v;
	struct VAR *l;
	struct VAR *r;
} var;

typedef struct VARMAT {
	struct VAR *vl[256];
} varmat;


varmat *varmat_alloc(void);
void varmat_free(varmat *);
int varmat_ngen(char *);
void value_free(var *);
var* var_alloc(var *, char *, unsigned int, value *);
var* var_find(var *, char *);
var* var_free(var *, char *);
void var_listfree(var *);
var* var_listadd(var *, var *);
var* v_alloc(varmat *, char *, unsigned int, value *);
var* v_find(varmat *, char *);
void v_free(varmat *, char *);
void v_freeall(varmat *);
varmat* v_list(varmat *, var *);
varmat* v_add(varmat *, varmat *);

char* str_type(unsigned int);
void varmat_print(varmat *);
void varlist_print(var *);
void var_print(var *);

#endif

