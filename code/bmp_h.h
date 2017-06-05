#ifndef	_bmp_h_h_
#define	_bmp_h_h_

typedef struct __attribute__ ((packed)) S_bmp_info {
	unsigned int biSize;
	signed int biWidth;
	signed int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCounts;
	unsigned int biCompression;
	unsigned int biSizeImage;
	signed int biXPelsPerMeter;
	signed int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} Bmp_info;

typedef struct __attribute__ ((packed)) S_bmp_hi {
	// head
	char bfTyte[2];
	unsigned int bfSize;
	unsigned int bfRes;
	unsigned int bfOffset;
	// info
	Bmp_info info;
} Bmp_hi;

typedef struct S_bmp {
	struct S_bmp_hi hi;
	unsigned int width;
	unsigned int height;
	unsigned int *data;
} Bmp;

typedef struct S_pxy {
	unsigned int x;
	unsigned int y;
	struct S_pxy *next;
} Pxy;

typedef struct S_fpxy {
	unsigned int y;
	unsigned int x1;
	unsigned int x2;
	struct S_fpxy *next;
} Fpxy;

Bmp* bmp_varget(var *, char *);
Bmp* Bmp_new(signed int , signed int);
Bmp* Bmp_load(char *);
int Bmp_write(Bmp *, char *);
void Bmp_clean(Bmp *, unsigned int);
void Bmp_set(Bmp *, unsigned int, unsigned int);
void Bmp_add(Bmp *, unsigned int, unsigned int);
void Bmp_set_alpha(Bmp *, unsigned int);
void Bmp_fill(Bmp *, unsigned int, unsigned int);
void Bmp_ffill(Bmp *, char *, unsigned int, unsigned int);
void Bmp_cset(Bmp *bmp, char *, unsigned int, unsigned int, unsigned int, unsigned int);
void Bmp_cadd(Bmp *bmp, char *, unsigned int, unsigned int, unsigned int, unsigned int);
void Bmp_fcset(Bmp *bmp, char *, char *, unsigned int, unsigned int, unsigned int, unsigned int);
void Bmp_fcadd(Bmp *bmp, char *, char *, unsigned int, unsigned int, unsigned int, unsigned int);
void Bmp_put_set(Bmp *, Bmp *, unsigned int, unsigned int);
void Bmp_put_add(Bmp *, Bmp *, unsigned int, unsigned int);
void Bmp_get_set(Bmp *, Bmp *, int, int);
void Bmp_get_add(Bmp *, Bmp *, int, int);
void Bmp_get_zoom(Bmp *, Bmp *);
void Bmp_rota(Bmp *, Bmp *, double, double, double);
void Bmp_revw(Bmp *);
void Bmp_revh(Bmp *);
void Bmp_arc_pra(Bmp *, double, double, double, double, double);
void Bmp_line(Bmp *, double, double, double, double);
void Bmp_arc_3p(Bmp *bmp, int, int, int, int, int, int);
void Bmp_arc_next(Bmp *, int, int);

#endif

