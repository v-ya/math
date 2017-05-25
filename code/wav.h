#ifndef	_wav_h_
#define	_wav_h_

#define	Time_max 60000

int *pTime;

#define	SamplingF 96000

Wav* Wav_new(unsigned int mtime)
{
	Wav *wav;
	unsigned int s;
	if (pTime==NULL) return NULL;
	if (mtime==0||mtime>*pTime) return NULL;
	wav=malloc(sizeof(Wav));
	if (!wav) return NULL;
	s=(int)(1.0*mtime*SamplingF/1000*2);
	wav->hi.RIFF[0]='R';
	wav->hi.RIFF[1]='I';
	wav->hi.RIFF[2]='F';
	wav->hi.RIFF[3]='F';
	wav->hi.fsz=36+s;
	wav->hi.WAVE[0]='W';
	wav->hi.WAVE[1]='A';
	wav->hi.WAVE[2]='V';
	wav->hi.WAVE[3]='E';
	wav->hi.Label_fmt[0]='f';
	wav->hi.Label_fmt[1]='m';
	wav->hi.Label_fmt[2]='t';
	wav->hi.Label_fmt[3]=' ';
	wav->hi.fmt_sz=16;
	wav->hi.format=1;
	wav->hi.channel=1;
	wav->hi.samp_f=SamplingF;
	wav->hi.samp_bf=SamplingF*2;
	wav->hi.align=2;
	wav->hi.depth=16;
	wav->hi.Label_data[0]='d';
	wav->hi.Label_data[1]='a';
	wav->hi.Label_data[2]='t';
	wav->hi.Label_data[3]='a';
	wav->hi.data_sz=s;
	wav->size=s/2;
	wav->data=malloc(s);
	if (wav->data)
	{
		memset(wav->data,0,s);
		return wav;
	}
	else
	{
		free(wav);
		return NULL;
	}
}

int Wav_write(Wav *wav, char *path)
{
	FILE *fp;
	int i;
	char *d;
	if ((!wav)||(!wav->data)) return -1;
	fp=fopen(path,"wb");
	if (!fp) return 1;
	fwrite(&(wav->hi),44,1,fp);
	fseek(fp,44,SEEK_SET);
	d=(char*)wav->data;
	for(i=wav->hi.data_sz;i>0;i--)
	putc(*(d++),fp);
	fclose(fp);
	return 0;
}

void Wav_set(Wav *wav, unsigned int t, unsigned int v)
{
	if (t<wav->size)
	wav->data[t]=(signed short)v;
}

#endif

