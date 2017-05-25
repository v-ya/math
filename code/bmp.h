#ifndef	_bmp_h_
#define	_bmp_h_

#define Width_max 2048
#define Height_max 1024

int *pWidth;
int *pHeight;
unsigned int *pColor;

Bmp* Bmp_new(signed int width, signed int height)
{
	Bmp *bmp;
	unsigned int w,h,s;
	if (pWidth==NULL||pHeight==NULL) return NULL;
	w=(width<0)?(-width):width;
	h=(height<0)?(-height):height;
	s=w*h*4;
	if (w==0||w>*pWidth||h==0||h>*pHeight) return NULL;
	bmp=malloc(sizeof(Bmp));
	if (!bmp) return NULL;
	bmp->hi.bfTyte[0]='B';
	bmp->hi.bfTyte[1]='M';
	bmp->hi.bfSize=64+s;
	bmp->hi.bfRes=0;
	bmp->hi.bfOffset=64;
	bmp->hi.biSize=40;
	bmp->hi.biWidth=width;
	bmp->hi.biHeight=height;
	bmp->hi.biPlanes=1;
	bmp->hi.biBitCounts=32;
	bmp->hi.biCompression=0;
	bmp->hi.biSizeImage=s;
	bmp->hi.biXPelsPerMeter=0;
	bmp->hi.biYPelsPerMeter=0;
	bmp->hi.biClrUsed=0;
	bmp->hi.biClrImportant=0;
	bmp->width=w;
	bmp->height=h;
	bmp->data=malloc(s);
	if (bmp->data)
	{
		memset(bmp->data,0xff,s);
		return bmp;
	}
	else
	{
		free(bmp);
		return NULL;
	}
}

int Bmp_write(Bmp *bmp, char *path)
{
	FILE *fp;
	int i;
	char *d;
	if ((!bmp)||(!bmp->data)) return -1;
	fp=fopen(path,"wb");
	if (!fp) return 1;
	fwrite(&(bmp->hi),54,1,fp);
	fseek(fp,bmp->hi.bfOffset,SEEK_SET);
	d=(char*)bmp->data;
	for(i=bmp->hi.biSizeImage;i>0;i--)
	putc(*(d++),fp);
	fclose(fp);
	return 0;
}

void Bmp_set(Bmp *bmp, unsigned int w, unsigned int h)
{
	if (bmp&&w<bmp->width&&h<bmp->height)
	bmp->data[h*bmp->width+w]=*pColor;
}

void Bmp_add(Bmp *bmp, unsigned int w, unsigned int h)
{
	unsigned int c;
	static double b1,g1,r1,a1,b2,g2,r2,a2;
	if (bmp&&w<bmp->width&&h<bmp->height)
	{
		c=bmp->data[h*bmp->width+w];
		b1=(c&0xff)/255.0;
		c>>=8;
		g1=(c&0xff)/255.0;
		c>>=8;
		r1=(c&0xff)/255.0;
		c>>=8;
		a1=(c&0xff)/255.0;
		c=*pColor;
		b2=(c&0xff)/255.0;
		c>>=8;
		g2=(c&0xff)/255.0;
		c>>=8;
		r2=(c&0xff)/255.0;
		c>>=8;
		a2=(c&0xff)/255.0;
		c=(unsigned int)((a1+a2-a1*a2)*255+0.5);
		a1*=(1-a2);
		b1=b2*a2+b1*a1;
		g1=g2*a2+g1*a1;
		r1=r2*a2+r1*a1;
		c<<=8;
		c|=(unsigned int)(r1*255+0.5);
		c<<=8;
		c|=(unsigned int)(g1*255+0.5);
		c<<=8;
		c|=(unsigned int)(b1*255+0.5);
		bmp->data[h*bmp->width+w]=c;
	}
}

void Bmp_set_alpha(Bmp *bmp, unsigned int alpha)
{
	int i;
	char *d;
	if (bmp&&bmp->data)
	{
		if (alpha<0x100) ;
		else alpha=255;
		d=(char*)bmp->data;
		d+=3;
		for(i=bmp->width*bmp->height-1;i>=0;i--)
		{
			*d=alpha;
			d+=4;
		}
	}
}

static Pxy *pxy=NULL;

void Pxy_push(unsigned int x, unsigned int y)
{
	Pxy *p;
	p=malloc(sizeof(Pxy));
	if (p)
	{
		p->x=x;
		p->y=y;
		p->next=pxy;
		pxy=p;
	}
}

void Pxy_pop(unsigned int *px, unsigned int *py)
{
	Pxy *p;
	if (pxy)
	{
		p=pxy;
		pxy=pxy->next;
		*px=p->x;
		*py=p->y;
		free(p);
	}
}

void Bmp_fill(Bmp *bmp, unsigned int w, unsigned int h)
{
	unsigned int wm,n,m,i,*d,c;
	if (bmp&&w<bmp->width&&h<bmp->height) ;
	else return ;
	wm=bmp->width;
	d=bmp->data;
	c=d[h*wm+w];
	if (c==*pColor) return ;
	Pxy_push(w,h);
	while(pxy)
	{
		Pxy_pop(&w,&h);
		i=h*wm;
		while(d[i+w]==c&&w) w--;
		if (d[i+w]!=c) w++;
		n=w;
		while(d[i+w]==c&&w<wm)
		{
			d[i+w]=*pColor;
			w++;
		}
		w--;
		m=w;
		if (h>0)
		{
			i=(h-1)*wm;
			if (n==m&&d[i+n]==c) Pxy_push(n,h-1);
			for(w=m;w>n;)
			{
				while(d[i+w]!=c&&w>n) w--;
				if (d[i+w]==c)
				{
					Pxy_push(w,h-1);
					while(d[i+w]==c&&w>n) w--;
				}
			}
		}
		if ((h+1)<bmp->height)
		{
			i=(h+1)*wm;
			if (n==m&&d[i+n]==c) Pxy_push(n,h+1);
			for(w=m;w>n;)
			{
				while(d[i+w]!=c&&w>n) w--;
				if (d[i+w]==c)
				{
					Pxy_push(w,h+1);
					while(d[i+w]==c&&w>n) w--;
				}
			}
		}
	}
}

void Bmp_block_set(Bmp *bmp, Bmp *bmp2, unsigned int w, unsigned int h)
{
	int wa,wb,ha,hb;
	unsigned int *d,*d2,i,i2;
	if (bmp&&bmp2)
	{
		d=bmp->data;
		d2=bmp2->data;
		wa=w;
		ha=h;
		wb=bmp2->width;
		hb=bmp2->height;
		if ((wb+wa)>bmp->width) wb=bmp->width-wa;
		if ((hb+ha)>bmp->height) hb=bmp->height-ha;
		if (wb<=0||hb<=0) return ;
		for(h=0;h<hb;h++)
		{
			i=(ha+h)*bmp->width+wa;
			i2=h*bmp2->width;
			for(w=0;w<wb;w++)
			d[i+w]=d2[i2+w];
		}
	}
}

void Bmp_block_add(Bmp *bmp, Bmp *bmp2, unsigned int w, unsigned int h)
{
	int wa,wb,ha,hb;
	unsigned int *d,*d2,i,i2;
	static double b1,g1,r1,a1,b2,g2,r2,a2;
	if (bmp&&bmp2)
	{
		d=bmp->data;
		d2=bmp2->data;
		wa=w;
		ha=h;
		wb=bmp2->width;
		hb=bmp2->height;
		if ((wb+wa)>bmp->width) wb=bmp->width-wa;
		if ((hb+ha)>bmp->height) hb=bmp->height-ha;
		if (wb<=0||hb<=0) return ;
		for(h=0;h<hb;h++)
		{
			i=(ha+h)*bmp->width+wa;
			i2=h*bmp2->width;
			for(w=0;w<wb;w++)
			{
				unsigned int c;
				c=d[i+w];
				b1=(c&0xff)/255.0;
				c>>=8;
				g1=(c&0xff)/255.0;
				c>>=8;
				r1=(c&0xff)/255.0;
				c>>=8;
				a1=(c&0xff)/255.0;
				c=d2[i2+w];
				b2=(c&0xff)/255.0;
				c>>=8;
				g2=(c&0xff)/255.0;
				c>>=8;
				r2=(c&0xff)/255.0;
				c>>=8;
				a2=(c&0xff)/255.0;
				c=(unsigned int)((a1+a2-a1*a2)*255+0.5);
				a1*=(1-a2);
				b1=b2*a2+b1*a1;
				g1=g2*a2+g1*a1;
				r1=r2*a2+r1*a1;
				c<<=8;
				c|=(unsigned int)(r1*255+0.5);
				c<<=8;
				c|=(unsigned int)(g1*255+0.5);
				c<<=8;
				c|=(unsigned int)(b1*255+0.5);
				d[i+w]=c;
			}
		}
	}
}

#endif

