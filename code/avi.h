#ifndef	_avi_h_
#define	_avi_h_

#define Width_max 1920
#define Height_max 1080
#define FPS_max 60

int *pWidth;
int *pHeight;
int *pFps;

Avi* Avi_new(char *path, signed int width, signed int height, unsigned int fps)
{
	Avi *avi;
	unsigned int w,h,s;
	if (pWidth==NULL||pHeight==NULL||pFps==NULL) return NULL;
	w=(width<0)?(-width):width;
	h=(height<0)?(-height):height;
	s=w*h*4;
	if (w==0||w>*pWidth||h==0||h>*pHeight||fps==0||fps>*pFps) return NULL;
	avi=malloc(sizeof(Avi));
	if (!avi) return NULL;
	avi->hi.RIFF[0]='R';
	avi->hi.RIFF[1]='I';
	avi->hi.RIFF[2]='F';
	avi->hi.RIFF[3]='F';
	avi->hi.fsz=sizeof(Avi_hi)-8;
	avi->hi.AVI[0]='A';
	avi->hi.AVI[1]='V';
	avi->hi.AVI[2]='I';
	avi->hi.AVI[3]=' ';
	avi->hi.LIST_i[0]='L';
	avi->hi.LIST_i[1]='I';
	avi->hi.LIST_i[2]='S';
	avi->hi.LIST_i[3]='T';
	avi->hi.LIST_i_sz=16+sizeof(Avi_avih)+sizeof(Avi_strh)+sizeof(Avi_strf);
	avi->hi.hdrl[0]='h';
	avi->hi.hdrl[1]='d';
	avi->hi.hdrl[2]='r';
	avi->hi.hdrl[3]='l';
	// avih
	avi->hi.avih.avih[0]='a';
	avi->hi.avih.avih[1]='v';
	avi->hi.avih.avih[2]='i';
	avi->hi.avih.avih[3]='h';
	avi->hi.avih.size=sizeof(Avi_avih)-8;
	avi->hi.avih.msPF=ceil((double)1e6/fps);
	avi->hi.avih.maxBPS=fps*s;
	avi->hi.avih.PG=0;
	avi->hi.avih.flags=0x0910;
	avi->hi.avih.totalFs=0;
	avi->hi.avih.initFs=0;
	avi->hi.avih.Streams=1;
	avi->hi.avih.sugBfsz=s;
	avi->hi.avih.width=w;
	avi->hi.avih.height=h;
	avi->hi.avih.res[0]=0;
	avi->hi.avih.res[1]=0;
	avi->hi.avih.res[2]=0;
	avi->hi.avih.res[3]=0;
	// strl
	avi->hi.LIST_s[0]='L';
	avi->hi.LIST_s[1]='I';
	avi->hi.LIST_s[2]='S';
	avi->hi.LIST_s[3]='T';
	avi->hi.LIST_s_sz=4+sizeof(Avi_strh)+sizeof(Avi_strf);
	avi->hi.strl[0]='s';
	avi->hi.strl[1]='t';
	avi->hi.strl[2]='r';
	avi->hi.strl[3]='l';
	// strh
	avi->hi.strh.strh[0]='s';
	avi->hi.strh.strh[1]='t';
	avi->hi.strh.strh[2]='r';
	avi->hi.strh.strh[3]='h';
	avi->hi.strh.size=sizeof(Avi_strh)-8;
	avi->hi.strh.type[0]='v';
	avi->hi.strh.type[1]='i';
	avi->hi.strh.type[2]='d';
	avi->hi.strh.type[3]='s';
	avi->hi.strh.handler[0]=0;
	avi->hi.strh.handler[1]=0;
	avi->hi.strh.handler[2]=0;
	avi->hi.strh.handler[3]=0;
	avi->hi.strh.flags=0;
	avi->hi.strh.pri=0;
	avi->hi.strh.lang=0;
	avi->hi.strh.initFs=0;
	avi->hi.strh.scale=1;
	avi->hi.strh.rate=fps;
	avi->hi.strh.start=0;
	avi->hi.strh.length=0;
	avi->hi.strh.sugBfsz=s;
	avi->hi.strh.quality=~0;
	avi->hi.strh.SAMPsz=0;
	avi->hi.strh.rcFrame.left=0;
	avi->hi.strh.rcFrame.top=0;
	avi->hi.strh.rcFrame.right=w;
	avi->hi.strh.rcFrame.bottom=h;
	// strf
	avi->hi.strf.strf[0]='s';
	avi->hi.strf.strf[1]='t';
	avi->hi.strf.strf[2]='r';
	avi->hi.strf.strf[3]='f';
	avi->hi.strf.size=sizeof(Avi_strf)-8;
	avi->hi.strf.info.biSize=40;
	avi->hi.strf.info.biWidth=width;
	avi->hi.strf.info.biHeight=height;
	avi->hi.strf.info.biPlanes=1;
	avi->hi.strf.info.biBitCounts=32;
	avi->hi.strf.info.biCompression=0;
	avi->hi.strf.info.biSizeImage=s;
	avi->hi.strf.info.biXPelsPerMeter=0;
	avi->hi.strf.info.biYPelsPerMeter=0;
	avi->hi.strf.info.biClrUsed=0;
	avi->hi.strf.info.biClrImportant=0;
	// movi
	avi->hi.LIST_d[0]='L';
	avi->hi.LIST_d[1]='I';
	avi->hi.LIST_d[2]='S';
	avi->hi.LIST_d[3]='T';
	avi->hi.LIST_d_sz=4;
	avi->hi.movi[0]='m';
	avi->hi.movi[1]='o';
	avi->hi.movi[2]='v';
	avi->hi.movi[3]='i';
	// struct
	avi->width=w;
	avi->height=h;
	avi->fsz=s;
	avi->fps=fps;
	avi->totalFs=0;
	avi->limitFs=((unsigned int)((~0)-sizeof(Avi_hi)))/(s+8);
	avi->offset=sizeof(Avi_hi);
	avi->fp=fopen(path,"wb");
	
	if (avi->fp) return avi;
	else
	{
		free(avi);
		return NULL;
	}
}

int Avi_write(Avi *avi, Bmp *bmp, unsigned int nf)
{
	static char db[5]="00dc";
	unsigned int offset,size,i;
	char *d;
	if (avi->width!=bmp->width||avi->height!=bmp->height) return 1;
	if (nf>avi->totalFs) return 2;
	if (nf>=avi->limitFs) return 3;
	size=avi->fsz;
	offset=(size+8)*nf+avi->offset;
	if ((!avi->fp)||(!bmp->data)) return -1;
	fseek(avi->fp,offset,SEEK_SET);
	fwrite(db,4,1,avi->fp);
	fwrite(&size,4,1,avi->fp);
	d=(char*)bmp->data;
	for(i=0;i<size;i++) putc(d[i],avi->fp);
	avi->hi.fsz+=size+8;
	avi->hi.avih.totalFs++;
	avi->hi.strh.length++;
	avi->hi.LIST_d_sz+=size+8;
	avi->totalFs++;
	return 0;
}

#endif

