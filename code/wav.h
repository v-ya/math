#ifndef	_wav_h_
#define	_wav_h_

#define	Time_max 60000

int *pTime;

#define	SamplingF 96000
#define	SamplingFms 96

Wav* Wav_new(unsigned int mtime)
{
	Wav *wav;
	unsigned int s;
	if (pTime==NULL) return NULL;
	if (mtime==0||mtime>*pTime) return NULL;
	wav=malloc(sizeof(Wav));
	if (!wav) return NULL;
	s=(int)(1.0*mtime*SamplingFms*2);
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

Wav* Wav_load(char *path)
{
	Wav *wav;
	unsigned int s;
	FILE *fp;
	char *src;
	fp=fopen(path,"rb");
	if (!fp) return NULL;
	wav=malloc(sizeof(Wav));
	if (!wav) goto err;
	fread(&(wav->hi),sizeof(Wav_hi),1,fp);
	if (wav->hi.align!=2||wav->hi.depth!=16) goto err;
	if (wav->hi.format!=1||wav->hi.channel!=1) goto err;
	if (wav->hi.samp_f!=SamplingF||wav->hi.samp_bf!=SamplingF*2) goto err;
	s=wav->hi.data_sz;
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
	wav->hi.Label_data[0]='d';
	wav->hi.Label_data[1]='a';
	wav->hi.Label_data[2]='t';
	wav->hi.Label_data[3]='a';
	wav->size=s/2;
	wav->data=malloc(s);
	if (!wav->data) goto err;
	src=(char*)wav->data;
	for(;s>0;s--)
	*(src++)=getc(fp);
	fclose(fp);
	return wav;
	err:
	free(wav);
	fclose(fp);
	return NULL;
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

void Wav_add(Wav *wav, unsigned int t, unsigned int v)
{
	if (t<wav->size)
	wav->data[t]+=(signed short)v;
}

int Wav_getmax(Wav *wav, unsigned int t, unsigned int T)
{
	int r;
	unsigned int i;
	if (t>=wav->size) return 0;
	if (t+T>wav->size) T=wav->size-t;
	r=0;
	for(i=0;i<T;i++)
	{
		if (abs(wav->data[i+t])>abs(r))
		r=wav->data[i+t];
	}
	return r;
}

double Wav_getcos(Wav *wav, unsigned int n, unsigned int t, unsigned int T)
{
	double w,r;
	unsigned int i;
	if (t>=wav->size) return 0.0;
	w=2*M_PI/T*n;
	if (t+T>wav->size) T=wav->size-t;
	r=0;
	for(i=0;i<T;i++)
		r+=wav->data[i+t]*cos(w*i);
	if (n==0) r=r/T;
	else r=r*2/T;
	return r;
}

double Wav_getsin(Wav *wav, unsigned int n, unsigned int t, unsigned int T)
{
	double w,r;
	unsigned int i;
	if (t>=wav->size) return 0.0;
	w=2*M_PI/T*n;
	if (t+T>wav->size) T=wav->size-t;
	r=0;
	for(i=0;i<T;i++)
		r+=wav->data[i+t]*sin(w*i);
	r=r*2/T;
	return r;
}

void Wav_addcos(Wav *wav, unsigned int n, double K, unsigned int t, unsigned int T)
{
	double w,r;
	unsigned int i;
	if (t>=wav->size) return ;
	w=2*M_PI/T*n;
	if (t+T>wav->size) T=wav->size-t;
	for(i=0;i<T;i++)
	{
		r=K*cos(w*i);
		if (r<0) r-=0.5;
		else r+=0.5;
		wav->data[i+t]+=(signed int)r;
	}
	return ;
}

void Wav_addsin(Wav *wav, unsigned int n, double K, unsigned int t, unsigned int T)
{
	double w,r;
	unsigned int i;
	if (t>=wav->size) return ;
	w=2*M_PI/T*n;
	if (t+T>wav->size) T=wav->size-t;
	for(i=0;i<T;i++)
	{
		r=K*sin(w*i);
		if (r<0) r-=0.5;
		else r+=0.5;
		wav->data[i+t]+=(signed int)r;
	}
	return ;
}

double Wav_toleT(Wav *wav, unsigned int t0, unsigned int T0, unsigned int t1, unsigned int T1)
{
	double a1,a2,k,e,r;
	signed short *d;
	int i,j;
	if ((t0+T0>wav->size)||(t1+T1>wav->size)||T0==0||T1==0) return -1;
	a1=abs(Wav_getmax(wav,t0,T0));
	a2=abs(Wav_getmax(wav,t1,T1));
	k=T1;
	k/=T0;
	e=0;
	d=wav->data;
	for(i=0;i<T0;i++)
	{
		r=k*i;
		j=(int)r;
		r-=j;
		if ((t1+j+1)<wav->size) r=(d[t0+i]/a1)-(((1-r)*d[t1+j]+r*d[t1+j+1])/a2);
		else r=(d[t0+i]/a1)-(d[t1+j]/a2);
		e+=r*r;
	}
	e/=T0;
	return e;
}

unsigned int Wav_getT(Wav *wav, unsigned int t, unsigned int min, unsigned int max, double e)
{
	unsigned int x,m,t0,t1a,t1b;
	signed short *d;
	double ea,eb;
	m=wav->size;
	d=wav->data;
	x=t+min;
	while(x<m)
	{
		while(x<m)
		{
			x++;
			if (d[x]==0) break;
			else if (d[x-1]*d[x]<0) break;
		}
		t0=x-t;
		if (t0>max) return 0;
		t1b=x;
		x+=t0;
		while(x<m)
		{
			x++;
			if (d[x]==0) break;
			else if (d[x-1]*d[x]<0) break;
		}
		t1a=x-t1b;
		x=t1b+t0;
		while(x>t1b)
		{
			if (d[x]==0) break;
			else if (d[x-1]*d[x]<0) break;
			x--;
		}
		t1b=x-t1b;
		if (t1a>min&&t1a<max) ea=Wav_toleT(wav,t,t0,t+t0,t1a);
		else ea=-1;
		if (t1b>min&&t1b<max) eb=Wav_toleT(wav,t,t0,t+t0,t1b);
		else eb=-1;
		if ((ea>=0&&ea<e)||(eb>=0&&eb<e)) return t0;
		else x=t+t0;
	}
	return 0;
}

#endif

