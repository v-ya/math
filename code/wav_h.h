#ifndef	_wav_h_h_
#define	_wav_h_h_

typedef struct __attribute__ ((packed)) S_wav_hi {
	// head
	char RIFF[4];
	unsigned int fsz;
	char WAVE[4];
	// info
	char Label_fmt[4];
	unsigned int fmt_sz;
	unsigned short format;
	unsigned short channel;
	unsigned int samp_f;
	unsigned int samp_bf;
	unsigned short align;
	unsigned short depth;
	char Label_data[4];
	unsigned int data_sz;
} Wav_hi;

typedef struct S_wav {
	struct S_wav_hi hi;
	unsigned int size;
	signed short *data;
} Wav;

Wav* Wav_new(unsigned int);
int Wav_write(Wav *, char *);
void Wav_set(Wav *, unsigned int, unsigned int);

#endif

