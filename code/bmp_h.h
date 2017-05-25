#ifndef	_bmp_h_h_
#define	_bmp_h_h_

typedef struct __attribute__ ((packed)) S_bmp_hi {
	// head
	char bfTyte[2];
	unsigned int bfSize;
	unsigned int bfRes;
	unsigned int bfOffset;
	// info
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

int Bmp_write(Bmp *, char *);
void Bmp_set(Bmp *, unsigned int, unsigned int);
void Bmp_add(Bmp *, unsigned int, unsigned int);
void Bmp_set_alpha(Bmp *, unsigned int);
void Bmp_fill(Bmp *, unsigned int, unsigned int);
void Bmp_block_set(Bmp *, Bmp *, unsigned int, unsigned int);
void Bmp_block_add(Bmp *, Bmp *, unsigned int, unsigned int);

#endif

