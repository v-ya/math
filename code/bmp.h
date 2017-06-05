#ifndef	_bmp_h_
#define	_bmp_h_

#define Width_max 7680
#define Height_max 4320

int *pWidth;
int *pHeight;
unsigned int *pColor;
var *pVarX;
var *pVarY;
var *pVarT;
int *pSx;
int *pSy;
double *pSa;

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
	bmp->hi.info.biSize=40;
	bmp->hi.info.biWidth=width;
	bmp->hi.info.biHeight=height;
	bmp->hi.info.biPlanes=1;
	bmp->hi.info.biBitCounts=32;
	bmp->hi.info.biCompression=0;
	bmp->hi.info.biSizeImage=s;
	bmp->hi.info.biXPelsPerMeter=0;
	bmp->hi.info.biYPelsPerMeter=0;
	bmp->hi.info.biClrUsed=0;
	bmp->hi.info.biClrImportant=0;
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

Bmp* Bmp_load(char *path)
{
	Bmp *bmp;
	FILE *fp;
	char *d;
	unsigned int i,s,offset,depth;
	fp=fopen(path,"rb");
	if (!fp) return NULL;
	bmp=malloc(sizeof(Bmp));
	fread(&(bmp->hi),sizeof(Bmp_hi),1,fp);
	bmp->width=(bmp->hi.info.biWidth<0)?(-bmp->hi.info.biWidth):bmp->hi.info.biWidth;
	bmp->height=(bmp->hi.info.biHeight<0)?(-bmp->hi.info.biHeight):bmp->hi.info.biHeight;
	s=bmp->width*bmp->height*4;
	offset=bmp->hi.bfOffset;
	bmp->hi.bfSize=64+s;
	bmp->hi.bfRes=0;
	bmp->hi.bfOffset=64;
	bmp->hi.info.biPlanes=1;
	bmp->hi.info.biSizeImage=s;
	if (bmp->hi.bfTyte[0]!='B'||bmp->hi.bfTyte[1]!='M') goto err;
	if (bmp->hi.info.biSize!=40) goto err;
	if (bmp->hi.info.biCompression!=0) goto err;
	if (bmp->hi.info.biClrUsed!=0) goto err;
	if (bmp->hi.info.biClrImportant!=0) goto err;
	depth=bmp->hi.info.biBitCounts;
	bmp->hi.info.biBitCounts=32;
	if (depth!=32&&depth!=24) goto err;
	bmp->data=malloc(s);
	if (!bmp->data) goto err;
	d=(char*)bmp->data;
	fseek(fp,offset,SEEK_SET);
	switch(depth)
	{
		case 32:
			for(i=0;i<s;i++) d[i]=getc(fp);
			break;
		case 24:
			for(i=0;i<s;i++)
			{
				d[i++]=getc(fp);
				d[i++]=getc(fp);
				d[i++]=getc(fp);
				d[i]=0xff;
			}
			break;
	}
	fclose(fp);
	return bmp;
	err:
	free(bmp);
	fclose(fp);
	return NULL;
}

int Bmp_write(Bmp *bmp, char *path)
{
	FILE *fp;
	int i;
	char *d;
	if ((!bmp)||(!bmp->data)) return -1;
	fp=fopen(path,"wb");
	if (!fp) return 1;
	fwrite(&(bmp->hi),sizeof(Bmp_hi),1,fp);
	fseek(fp,bmp->hi.bfOffset,SEEK_SET);
	d=(char*)bmp->data;
	for(i=bmp->hi.info.biSizeImage;i>0;i--)
	putc(*(d++),fp);
	fclose(fp);
	return 0;
}

void Bmp_clean(Bmp *bmp, unsigned int color)
{
	unsigned int i,s;
	s=bmp->width*bmp->height;
	for(i=0;i<s;i++) bmp->data[i]=color;
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
static Fpxy *fpxy=NULL;

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

void Fpxy_push(unsigned int y, unsigned int x1, unsigned int x2)
{
	Fpxy *p;
	p=malloc(sizeof(Fpxy));
	if (p)
	{
		p->y=y;
		p->x1=x1;
		p->x2=x2;
		p->next=fpxy;
		fpxy=p;
	}
}

void Fpxy_pop(unsigned int *py, unsigned int *px1, unsigned int *px2)
{
	Fpxy *p;
	if (fpxy)
	{
		p=fpxy;
		fpxy=fpxy->next;
		*py=p->y;
		*px1=p->x1;
		*px2=p->x2;
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

void Bmp_ffill(Bmp *bmp, char* fc, unsigned int w, unsigned int h)
{
	unsigned int wm,n,m,i,*d,c;
	var v;
	if (pVarX==NULL||pVarY==NULL) return ;
	if (bmp&&w<bmp->width&&h<bmp->height) ;
	else return ;
	wm=bmp->width;
	d=bmp->data;
	c=d[h*wm+w];
	*pColor=~c;
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
		Fpxy_push(h,n,m);
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
	while(fpxy)
	{
		Fpxy_pop(&h,&n,&m);
		i=h*wm;
		for(w=n;w<=m;w++)
		{
			*pColor=d[i+w];
			pVarX->v.v_int=w;
			pVarY->v.v_int=h;
			cal(fc,&v);
			d[i+w]=(int)v.v.v_float;
		}
	}
}

void Bmp_cset(Bmp *bmp, char *con, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	unsigned int i,x,y,c,*d;
	var v;
	if (pVarX==NULL||pVarY==NULL) return ;
	if (x1>x2)
	{
		x=x1;
		x1=x2;
		x2=x;
	}
	if (y1>y2)
	{
		y=y1;
		y1=y2;
		y2=y;
	}
	if (x2>bmp->width) x2=bmp->width;
	if (y2>bmp->height) y2=bmp->height;
	d=bmp->data;
	c=*pColor;
	for(y=y1;y<y2;y++)
	{
		i=y*bmp->width;
		for(x=x1;x<x2;x++)
		{
			pVarX->v.v_int=x;
			pVarY->v.v_int=y;
			*pColor=d[i+x];
			cal(con,&v);
			if (v.v.v_float!=0) d[i+x]=c;
		}
	}
	*pColor=c;
}

void Bmp_cadd(Bmp *bmp, char *con, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	unsigned int i,x,y,cl,c,*d;
	static double b1,g1,r1,a1,b2,g2,r2,a2;
	var v;
	if (pVarX==NULL||pVarY==NULL) return ;
	if (x1>x2)
	{
		x=x1;
		x1=x2;
		x2=x;
	}
	if (y1>y2)
	{
		y=y1;
		y1=y2;
		y2=y;
	}
	if (x2>bmp->width) x2=bmp->width;
	if (y2>bmp->height) y2=bmp->height;
	d=bmp->data;
	cl=*pColor;
	for(y=y1;y<y2;y++)
	{
		i=y*bmp->width;
		for(x=x1;x<x2;x++)
		{
			pVarX->v.v_int=x;
			pVarY->v.v_int=y;
			*pColor=d[i+x];
			cal(con,&v);
			if (v.v.v_float!=0)
			{
				c=d[i+x];
				b1=(c&0xff)/255.0;
				c>>=8;
				g1=(c&0xff)/255.0;
				c>>=8;
				r1=(c&0xff)/255.0;
				c>>=8;
				a1=(c&0xff)/255.0;
				c=cl;
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
				d[i+x]=c;
			}
		}
	}
	*pColor=cl;
}

void Bmp_fcset(Bmp *bmp, char *con, char *fc, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	unsigned int i,x,y,c,*d;
	var v;
	if (pVarX==NULL||pVarY==NULL) return ;
	if (x1>x2)
	{
		x=x1;
		x1=x2;
		x2=x;
	}
	if (y1>y2)
	{
		y=y1;
		y1=y2;
		y2=y;
	}
	if (x2>bmp->width) x2=bmp->width;
	if (y2>bmp->height) y2=bmp->height;
	d=bmp->data;
	for(y=y1;y<y2;y++)
	{
		i=y*bmp->width;
		for(x=x1;x<x2;x++)
		{
			pVarX->v.v_int=x;
			pVarY->v.v_int=y;
			*pColor=d[i+x];
			cal(con,&v);
			if (v.v.v_float!=0)
			{
				cal(fc,&v);
				d[i+x]=(int)v.v.v_float;
			}
		}
	}
}

void Bmp_fcadd(Bmp *bmp, char *con, char *fc, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	unsigned int i,x,y,c,*d;
	static double b1,g1,r1,a1,b2,g2,r2,a2;
	var v;
	if (pVarX==NULL||pVarY==NULL) return ;
	if (x1>x2)
	{
		x=x1;
		x1=x2;
		x2=x;
	}
	if (y1>y2)
	{
		y=y1;
		y1=y2;
		y2=y;
	}
	if (x2>bmp->width) x2=bmp->width;
	if (y2>bmp->height) y2=bmp->height;
	d=bmp->data;
	for(y=y1;y<y2;y++)
	{
		i=y*bmp->width;
		for(x=x1;x<x2;x++)
		{
			pVarX->v.v_int=x;
			pVarY->v.v_int=y;
			*pColor=d[i+x];
			cal(con,&v);
			if (v.v.v_float!=0)
			{
				cal(fc,&v);
				c=d[i+x];
				b1=(c&0xff)/255.0;
				c>>=8;
				g1=(c&0xff)/255.0;
				c>>=8;
				r1=(c&0xff)/255.0;
				c>>=8;
				a1=(c&0xff)/255.0;
				c=(int)v.v.v_float;
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
				d[i+x]=c;
			}
		}
	}
}

void Bmp_put_set(Bmp *bmp, Bmp *bmp2, unsigned int w, unsigned int h)
{
	int wa,wb,ha,hb;
	unsigned int *d,*d2,i,i2;
	if (bmp&&bmp2&&w<bmp->width&&h<bmp->height)
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

void Bmp_put_add(Bmp *bmp, Bmp *bmp2, unsigned int w, unsigned int h)
{
	int wa,wb,ha,hb;
	unsigned int *d,*d2,i,i2;
	static double b1,g1,r1,a1,b2,g2,r2,a2;
	if (bmp&&bmp2&&w<bmp->width&&h<bmp->height)
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

void Bmp_get_set(Bmp *bmp, Bmp *bmp2, int w, int h)
{
	int x,y;
	unsigned int i,i2,*d,*d2;
	d=bmp->data;
	d2=bmp2->data;
	for(y=0;y<bmp->height;y++)
	{
		i=y*bmp->width;
		i2=(y+h)*bmp2->width+w;
		for(x=0;x<bmp->width;x++)
		{
			if ((y+h)>=0&&(y+h)<bmp2->height&&(x+w)>=0&&(x+w)<bmp2->width)
			d[i+x]=d2[i2+x];
			else d[i+x]=*pColor;
		}
	}
}

void Bmp_get_add(Bmp *bmp, Bmp *bmp2, int w, int h)
{
	int x,y;
	unsigned int i,i2,*d,*d2,c;
	static double b1,g1,r1,a1,b2,g2,r2,a2;
	d=bmp->data;
	d2=bmp2->data;
	for(y=0;y<bmp->height;y++)
	{
		i=y*bmp->width;
		i2=(y+h)*bmp2->width+w;
		for(x=0;x<bmp->width;x++)
		{
			if ((y+h)>=0&&(y+h)<bmp2->height&&(x+w)>=0&&(x+w)<bmp2->width)
			c=d2[i2+x];
			else c=*pColor;
			b2=(c&0xff)/255.0;
			c>>=8;
			g2=(c&0xff)/255.0;
			c>>=8;
			r2=(c&0xff)/255.0;
			c>>=8;
			a2=(c&0xff)/255.0;
			c=d[i+x];
			b1=(c&0xff)/255.0;
			c>>=8;
			g1=(c&0xff)/255.0;
			c>>=8;
			r1=(c&0xff)/255.0;
			c>>=8;
			a1=(c&0xff)/255.0;
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
			d[i+x]=c;
		}
	}
}

void Bmp_get_zoom(Bmp *bmp, Bmp *bmp2)
{
	unsigned i,i2,x,y,*d,*d2;
	double kx,ky;
	if (bmp->width<2||bmp->height<2) return ;
	kx=(double)(bmp2->width-1)/(bmp->width-1);
	ky=(double)(bmp2->height-1)/(bmp->height-1);
	d=bmp->data;
	d2=bmp2->data;
	for(y=0;y<bmp->height;y++)
	{
		i=y*bmp->width;
		i2=((int)(ky*y+0.5))*bmp2->width;
		for(x=0;x<bmp->width;x++)
		d[i+x]=d2[i2+(int)(kx*x+0.5)];
	}
}

void Bmp_rota(Bmp *bmp, Bmp *bmp2, double x, double y, double a)
{
	int i,j,k,w,h;
	unsigned int *d,*d2;
	double dx,dy,r,ta;
	Bmp_clean(bmp,*pColor);
	d=bmp->data;
	d2=bmp2->data;
	for(i=0;i<bmp->height;i++)
	{
		k=i*bmp->width;
		for(j=0;j<bmp->width;j++)
		{
			dx=(double)j-x;
			dy=(double)i-y;
			r=hypot(dx,dy);
			if (r>0)
			{
				if (dy>0) ta=acos(dx/r);
				else ta=-acos(dx/r);
				ta-=a;
				w=(int)(x+r*cos(ta)+0.5);
				h=(int)(y+r*sin(ta)+0.5);
			}
			else
			{
				w=(int)(x+0.5);
				h=(int)(y+0.5);
			}
			if (w>=0&&w<bmp2->width&&h>=0&&h<bmp2->height) d[k+j]=d2[h*bmp2->width+w];
		}
	}
}

void Bmp_revw(Bmp *bmp)
{
	unsigned int c,i,x,y,xm,*d;
	xm=bmp->width/2;
	d=bmp->data;
	for(y=0;y<bmp->height;y++)
	{
		i=y*bmp->width;
		for(x=0;x<xm;x++)
		{
			c=d[i+x];
			d[i+x]=d[i+bmp->width-x-1];
			d[i+bmp->width-x-1]=c;
		}
	}
}

void Bmp_revh(Bmp *bmp)
{
	unsigned int c,w,h,x,y,ym,*d;
	w=bmp->width;
	h=bmp->height-1;
	ym=bmp->height/2;
	d=bmp->data;
	for(x=0;x<w;x++)
	{
		for(y=0;y<ym;y++)
		{
			c=d[y*w+x];
			d[y*w+x]=d[(h-y)*w+x];
			d[(h-y)*w+x]=c;
		}
	}
}

void Bmp_arc_pra(Bmp *bmp, double rx, double ry, double r, double rad1, double rad2)
{
	double drad;
	unsigned int x,y,i,n;
	if (r==0) return ;
	drad=rad2-rad1;
	n=(unsigned int)ceil(fabs(drad*r));
	drad/=n;
	for(i=0;i<=n;i++)
	{
		x=(unsigned int)(rx+r*cos(rad1)+0.5);
		y=(unsigned int)(ry+r*sin(rad1)+0.5);
		if (x<bmp->width&&y<bmp->height) bmp->data[y*bmp->width+x]=*pColor;
		rad1+=drad;
	}
	if (pSx) *pSx=x;
	if (pSy) *pSy=y;
	if (pSa)
	{
		if (drad>0) *pSa=rad2+M_PI_2;
		else *pSa=rad2-M_PI_2;
	}
}

void Bmp_line(Bmp *bmp, double x1, double y1, double x2, double y2)
{
	double dx,dy;
	unsigned int x,y,i,n;
	dx=x2-x1;
	dy=y2-y1;
	if (dx==0&&dy==0) return ;
	if (fabs(dx)>fabs(dy)) n=ceil(fabs(dx));
	else n=ceil(fabs(dy));
	dx/=n;
	dy/=n;
	for(i=0;i<=n;i++)
	{
		x=(unsigned int)(x1+0.5);
		y=(unsigned int)(y1+0.5);
		if (x<bmp->width&&y<bmp->height) bmp->data[y*bmp->width+x]=*pColor;
		x1+=dx;
		y1+=dy;
	}
	if (pSx) *pSx=x;
	if (pSy) *pSy=y;
	if (pSa)
	{
		if (dx==0) *pSa=(dy>0)?M_PI_2:(-M_PI_2);
		else if (dx>0) *pSa=atan(dy/dx);
		else
		{
			*pSa=atan(dy/dx);
			if (*pSa>0) *pSa-=M_PI;
			else *pSa+=M_PI;
		}
	}
}

void Bmp_arc_3p(Bmp *bmp, int x1, int y1, int x2, int y2, int x3, int y3)
{
	double r1,r2,r3,tx,ty,r,a1,a2,a3;
	r=(double)x1*y2+x2*y3+x3*y1-x1*y3-x2*y1-x3*y2;
	if (r==0)
	{
		Bmp_line(bmp,x1,y1,x3,y3);
		return ;
	}
	r*=2;
	r1=x1*x1+y1*y1;
	r2=x2*x2+y2*y2;
	r3=x3*x3+y3*y3;
	tx=(double)r1*y2+r2*y3+r3*y1-r1*y3-r2*y1-r3*y2;
	ty=(double)x1*r2+x2*r3+x3*r1-x1*r3-x2*r1-x3*r2;
	tx/=r;
	ty/=r;
	r=hypot(tx-x2,ty-y2);
	a1=((double)x1-tx)/r;
	if (a1>1) a1=1;
	else if (a1<-1) a1=-1;
	if (y1>ty) a1=acos(a1);
	else a1=-acos(a1);
	a2=((double)x2-tx)/r;
	if (a2>1) a2=1;
	else if (a2<-1) a2=-1;
	if (y2>ty) a2=acos(a2);
	else a2=-acos(a2);
	a3=((double)x3-tx)/r;
	if (a3>1) a3=1;
	else if (a3<-1) a3=-1;
	if (y3>ty) a3=acos(a3);
	else a3=-acos(a3);
	if ((a3-a2)*(a2-a1)<0)
	{
		if (a1>a3) a1-=M_PI*2;
		else a1+=M_PI*2;
	}
	Bmp_arc_pra(bmp,tx,ty,r,a1,a3);
}

void Bmp_arc_next(Bmp *bmp, int x, int y)
{
	double l,tx,ty,a1,a2,r;
	if (pSx&&pSy&&pSa) ;
	else return ;
	tx=x-*pSx;
	ty=y-*pSy;
	l=hypot(tx,ty);
	if (l==0) return ;
	a2=acos(tx/l);
	if (ty<0) a2=-a2;
	r=cos(M_PI_2+*pSa-a2);
	if (r==0) Bmp_line(bmp,*pSx,*pSy,x,y);
	else
	{
		r=l/r/2;
		tx=*pSx+r*cos(*pSa+M_PI_2);
		ty=*pSy+r*sin(*pSa+M_PI_2);
		if (r>0)
		{
			a1=((double)*pSx-tx)/r;
			if (a1>1) a1=1;
			else if (a1<-1) a1=-1;
			if (*pSy>ty) a1=acos(a1);
			else a1=-acos(a1);
			a2=((double)x-tx)/r;
			if (a2>1) a2=1;
			else if (a2<-1) a2=-1;
			if (y>ty) a2=acos(a2);
			else a2=-acos(a2);
			if (a1>a2) a2+=M_PI*2;
			Bmp_arc_pra(bmp,tx,ty,r,a1,a2);
		}
		else
		{
			r=-r;
			a1=((double)*pSx-tx)/r;
			if (a1>1) a1=1;
			else if (a1<-1) a1=-1;
			if (*pSy>ty) a1=acos(a1);
			else a1=-acos(a1);
			a2=((double)x-tx)/r;
			if (a2>1) a2=1;
			else if (a2<-1) a2=-1;
			if (y>ty) a2=acos(a2);
			else a2=-acos(a2);
			if (a1<a2) a1+=M_PI*2;
			Bmp_arc_pra(bmp,tx,ty,r,a1,a2);
		}
	}
}

#endif

