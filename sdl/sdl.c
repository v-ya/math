#include "../dl/dl.h"
#include "sdl.h"
#include "../code/bmp_h.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

static char *model_name=ModelName;
static char *eventname[16]={
	"NOEVENT",
	"ACTIVEEVENT",
	"KEYDOWN",
	"KEYUP",
	"MOUSEMOTION",
	"MOUSEBUTTONDOWN",
	"MOUSEBUTTONUP",
	"JOYAXISMOTION",
	"JOYBALLMOTION",
	"JOYHATMOTION",
	"JOYBUTTONDOWN",
	"JOYBUTTONUP",
	"QUIT",
	"SYSWMEVENT",
	"VIDEORESIZE",
	"VIDEOEXPOSE"
};

var *_event;
SDL_Surface *screen;
int w,h,loop;
var *vbmp;
SDL_Event event;
Work work;
unsigned char *KeyState;
unsigned int KeyNum;
SDL_Thread *fps_thread;
var *fps_var;

_fun(model_name)
{
	ret->mode=type_string;
	ret->v.v_string=model_name;
}

_fun(SDL_open)
{
	int is_resize;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n<2)
	{
		err:
		w=0;
		h=0;
		dp("[%s].open: 传入非法参数\n",model_name);
		return ;
	}
	w=_int(argv(0));
	h=_int(argv(1));
	if (n>2) is_resize=_int(argv(2));
	else is_resize=0;
	if (w<=0||h<=0) goto err;
	if (is_resize) screen=SDL_SetVideoMode(w,h,32,SDL_SWSURFACE|SDL_RESIZABLE);
	else screen=SDL_SetVideoMode(w,h,32,SDL_SWSURFACE);
	if (!screen)
	{
		w=0;
		h=0;
		if (vbmp)
		{
			vbmp->mode&=(~auth_noset)&(~auth_norev);
			fun->var_free(vbmp->v.v_object,"Type");
			fun->var_free(vbmp->v.v_object,"_data");
			fun->var_free(vbmp->v.v_object,"Width");
			fun->var_free(vbmp->v.v_object,"Height");
			vbmp=NULL;
		}
		dp("[%s].open: 设置窗口失败\n",model_name);
		return ;
	}
	if (vbmp)
	{
		var *vp;
		Bmp *bmp;
		unsigned int s;
		vp=fun->var_find(vbmp->v.v_object,"_data");
		bmp=vp->v.v_void;
		s=w*h*4;
		bmp->hi.bfSize=64+s;
		bmp->hi.info.biWidth=w;
		bmp->hi.info.biHeight=-h;
		bmp->width=w;
		bmp->height=h;
		bmp->data=screen->pixels;
		vp=fun->var_find(vbmp->v.v_object,"Width");
		vp->v.v_int=w;
		vp=fun->var_find(vbmp->v.v_object,"Height");
		vp->v.v_int=h;
	}
}

_fun(SDL_close)
{
	if (vbmp)
	{
		vbmp->mode&=(~auth_noset)&(~auth_norev);
		fun->var_free(vbmp->v.v_object,"Type");
		fun->var_free(vbmp->v.v_object,"_data");
		fun->var_free(vbmp->v.v_object,"Width");
		fun->var_free(vbmp->v.v_object,"Height");
		vbmp=NULL;
	}
	SDL_Quit();
	screen=NULL;
	ret->mode=type_void;
	ret->v.v_void=NULL;
}

_fun(SDL_delay)
{
	int t;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (!screen) return ;
	if (n<1)
	{
		dp("[%s].delay: 传入非法参数\n",model_name);
		return ;
	}
	t=_int(argv(0));
	if (t>0) SDL_Delay(t);
}

_fun(SDL_update)
{
	int x,y,rw,rh;
	if (n>=4)
	{
		while(vlist->l) vlist=vlist->l;
		x=_int(vlist);
		vlist=vlist->r;
		y=_int(vlist);
		vlist=vlist->r;
		rw=_int(vlist);
		vlist=vlist->r;
		rh=_int(vlist);
		if (x<0|x>=w|y<0|y>=h|rw<0|rw>w|x+rw>w|rh<0|rh>h|y+rh>h) SDL_UpdateRect(screen,0,0,0,0);
		else SDL_UpdateRect(screen,x,y,rw,rh);
		
	}
	else
	{
		SDL_UpdateRect(screen,0,0,0,0);
	}
}

int _thread_fps(var *fps)
{
	unsigned int time=0,t=0,pt=0;
	while(screen&&fps->v.v_float>0)
	{
		t=SDL_GetTicks();
		if (time+pt<=t)
		{
			time=t;
			SDL_UpdateRect(screen,0,0,0,0);
		}
		pt=(int)(1000/fps->v.v_float+0.5);
		SDL_Delay(1);
	}
	fps->v.v_float=0;
	fps_thread=NULL;
	return 0;
}

_fun(set_fps)
{
	double fps;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (fps_var)
	{
		if (n>0) fps=_float(argv(0));
		else fps=0;
		if (fps<0) fps=0;
		fps_var->v.v_float=fps;
		if (fps&&fps_thread==NULL)
		{
			fps_thread=SDL_CreateThread((void*)_thread_fps,fps_var);
		}
	}
}

_fun(get_bmp)
{
	var *vl,v;
	char *name;
	unsigned int s;
	Bmp *bmp;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (vbmp)
	{
		vbmp->mode&=(~auth_noset)&(~auth_norev);
		fun->var_free(vbmp->v.v_object,"Type");
		fun->var_free(vbmp->v.v_object,"_data");
		fun->var_free(vbmp->v.v_object,"Width");
		fun->var_free(vbmp->v.v_object,"Height");
		vbmp=NULL;
	}
	if (n>0&&screen)
	{
		vl=argv(0);
		name=_string(vl);
		if (!name)
		{
			dp("[%s].get_bmp: 传入非法参数\n",model_name);
			return ;
		}
		fun->get_var(name,&vbmp,&v);
		if (!vbmp)
		{
			err:
			dp("[%s].get_bmp: 不能找到对象 %s\n",model_name,name);
			return ;
		}
		if (!fun->is_obj(vbmp)) goto err;
		if (m_auth_rev(vbmp->mode)||m_auth_set(vbmp->mode))
		{
			dp("[%s].get_bmp: 对象 %s 没有足够权限\n",model_name,name);
			vbmp=NULL;
			return ;
		}
		// 设置结构
		s=w*h*4;
		bmp=malloc(sizeof(Bmp));
		if (!bmp) goto err_alloc;
		bmp->hi.bfTyte[0]='B';
		bmp->hi.bfTyte[1]='M';
		bmp->hi.bfSize=64+s;
		bmp->hi.bfRes=0;
		bmp->hi.bfOffset=64;
		bmp->hi.info.biSize=40;
		bmp->hi.info.biWidth=w;
		bmp->hi.info.biHeight=-h;
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
		bmp->data=screen->pixels;
		// 申请变量
		vl=fun->var_ralloc(vbmp->v.v_object,"_data",type_void|auth_noset|auth_norev,NULL);
		if (!vl) goto err_alloc;
		vl->v.v_void=bmp;
		vl->mode|=free_need;
		vl=fun->var_ralloc(vbmp->v.v_object,"Width",type_int|auth_noset|auth_norev,NULL);
		if (!vl) goto err_alloc2;
		vl->v.v_int=w;
		vl=fun->var_ralloc(vbmp->v.v_object,"Height",type_int|auth_noset|auth_norev,NULL);
		if (!vl) goto err_alloc2;
		vl->v.v_int=h;
		if (fun->obj_type(vbmp,"Type","Bmp")) goto err_alloc2;
		vbmp->mode|=auth_noset|auth_norev;
		return ;
		err_alloc:
		if (bmp) free(bmp);
		err_alloc2:
		dp("[%s].get_bmp: 申请内存失败\n",model_name);
		return ;
	}
}

_fun(set_event)
{
	char *en,*ef,**pef;
	ret->mode=type_int;
	ret->v.v_int=1;
	en=NULL;
	ef=NULL;
	if (n<1)
	{
		err:
		dp("[%s].set_event: 传入非法参数\n",model_name);
		return ;
	}
	vlist=argv(0);
	en=_string(vlist);
	if (!en) goto err;
	if (n>1)
	{
		vlist=argv(1);
		ef=_string(vlist);
	}
	if (strcmp(eventname[0],en)==0) pef=&work.noevent;
	else if (strcmp(eventname[1],en)==0) pef=&work.activeevent;
	else if (strcmp(eventname[2],en)==0) pef=&work.keydown;
	else if (strcmp(eventname[3],en)==0) pef=&work.keyup;
	else if (strcmp(eventname[4],en)==0) pef=&work.mousemotion;
	else if (strcmp(eventname[5],en)==0) pef=&work.mousebuttondown;
	else if (strcmp(eventname[6],en)==0) pef=&work.mousebuttonup;
	else if (strcmp(eventname[7],en)==0) pef=&work.joyaxismotion;
	else if (strcmp(eventname[8],en)==0) pef=&work.joyballmotion;
	else if (strcmp(eventname[9],en)==0) pef=&work.joyhatmotion;
	else if (strcmp(eventname[10],en)==0) pef=&work.joybuttondown;
	else if (strcmp(eventname[11],en)==0) pef=&work.joybuttonup;
	else if (strcmp(eventname[12],en)==0) pef=&work.quit;
	else if (strcmp(eventname[13],en)==0) pef=&work.syswmevent;
	else if (strcmp(eventname[14],en)==0) pef=&work.videoresize;
	else if (strcmp(eventname[15],en)==0) pef=&work.videoexpose;
	else
	{
		dp("[%s].set_event: 不能找到事件 %s\n",model_name,en);
		return ;
	}
	if (*pef) free(*pef);
	if (ef)
	{
		*pef=malloc(strlen(ef)+1);
		if (*pef) strcpy(*pef,ef);
		else
		{
			dp("[%s].set_event: 申请空间失败(at %s)\n",model_name,en);
			return ;
		}
	}
	else *pef=NULL;
	ret->v.v_int=0;
}

int work_event(void)
{
	var *vp,v;
	vp=NULL;
	switch(event.type)
	{
		case SDL_NOEVENT:
			if (work.noevent) fun->get_var(work.noevent,&vp,&v);
			break;
		case SDL_ACTIVEEVENT:
			if (work.activeevent) fun->get_var(work.activeevent,&vp,&v);
			break;
		case SDL_KEYDOWN:
			if (work.keydown) fun->get_var(work.keydown,&vp,&v);
			break;
		case SDL_KEYUP:
			if (work.keyup) fun->get_var(work.keyup,&vp,&v);
			break;
		case SDL_MOUSEMOTION:
			if (work.mousemotion) fun->get_var(work.mousemotion,&vp,&v);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (work.mousebuttondown) fun->get_var(work.mousebuttondown,&vp,&v);
			break;
		case SDL_MOUSEBUTTONUP:
			if (work.mousebuttonup) fun->get_var(work.mousebuttonup,&vp,&v);
			break;
		case SDL_JOYAXISMOTION:
			if (work.joyaxismotion) fun->get_var(work.joyaxismotion,&vp,&v);
			break;
		case SDL_JOYBALLMOTION:
			if (work.joyballmotion) fun->get_var(work.joyballmotion,&vp,&v);
			break;
		case SDL_JOYHATMOTION:
			if (work.joyhatmotion) fun->get_var(work.joyhatmotion,&vp,&v);
			break;
		case SDL_JOYBUTTONDOWN:
			if (work.joybuttondown) fun->get_var(work.joybuttondown,&vp,&v);
			break;
		case SDL_JOYBUTTONUP:
			if (work.joybuttonup) fun->get_var(work.joybuttonup,&vp,&v);
			break;
		case SDL_QUIT:
			if (work.quit) fun->get_var(work.quit,&vp,&v);
			break;
		case SDL_SYSWMEVENT:
			if (work.syswmevent) fun->get_var(work.syswmevent,&vp,&v);
			break;
		case SDL_VIDEORESIZE:
			if (work.videoresize) fun->get_var(work.videoresize,&vp,&v);
			break;
		case SDL_VIDEOEXPOSE:
			if (work.videoexpose) fun->get_var(work.videoexpose,&vp,&v);
			break;
	}
	if (vp) return _int(vp);
	else return 0;
}

_fun(wait_event)
{
	ret->mode=type_int;
	event.type=SDL_NOEVENT;
	SDL_WaitEvent(&event);
	ret->v.v_int=work_event();
}

_fun(poll_event)
{
	ret->mode=type_int;
	event.type=SDL_NOEVENT;
	SDL_PollEvent(&event);
	ret->v.v_int=work_event();
}

_fun(in_loop)
{
	int t;
	loop=1;
	if (n==0)
	{
		while(screen&&loop)
		{
			event.type=SDL_NOEVENT;
			SDL_WaitEvent(&event);
			work_event();
		}
	}
	else
	{
		t=_int(argv(0));
		while(screen&&loop)
		{
			event.type=SDL_NOEVENT;
			SDL_PollEvent(&event);
			work_event();
			if (t>0) SDL_Delay(t);
		}
	}
	ret->mode=type_void;
	ret->v.v_void=NULL;
}

_fun(out_loop)
{
	loop=0;
	ret->mode=type_void;
	ret->v.v_void=NULL;
}

_fun(set_title)
{
	char *title,*icon;
	if (n==0) return ;
	vlist=argv(0);
	title=_string(vlist);
	if (n>1)
	{
		vlist=argv(1);
		icon=_string(vlist);
	}
	else icon=NULL;
	SDL_WM_SetCaption(title,icon);
	ret->mode=type_void;
	ret->v.v_void=NULL;
}

_fun(get_key)
{
	unsigned int k;
	ret->mode=type_int;
	ret->v.v_int=0;
	if (n==0) return ;
	k=_int(argv(0));
	if (k<KeyNum) ret->v.v_int=KeyState[k];
}

// [sdl].event.*

_fun(event_type)
{
	ret->mode=type_int;
	ret->v.v_int=event.type;
}

_fun(event_Act_gain)
{
	ret->mode=type_int;
	ret->v.v_int=event.active.gain;
}

_fun(event_Act_state)
{
	ret->mode=type_int;
	ret->v.v_int=event.active.state;
}

_fun(event_Key_which)
{
	ret->mode=type_int;
	ret->v.v_int=event.key.which;
}

_fun(event_Key_state)
{
	ret->mode=type_int;
	ret->v.v_int=event.key.state;
}

_fun(event_Key_key)
{
	ret->mode=type_int;
	ret->v.v_int=event.key.keysym.sym;
}

_fun(event_Key_mod)
{
	ret->mode=type_int;
	ret->v.v_int=event.key.keysym.mod;
}

_fun(event_MouMo_which)
{
	ret->mode=type_int;
	ret->v.v_int=event.motion.which;
}

_fun(event_MouMo_state)
{
	ret->mode=type_int;
	ret->v.v_int=event.motion.state;
}

_fun(event_MouMo_x)
{
	ret->mode=type_int;
	ret->v.v_int=event.motion.x;
}

_fun(event_MouMo_y)
{
	ret->mode=type_int;
	ret->v.v_int=event.motion.y;
}

_fun(event_MouMo_xrel)
{
	ret->mode=type_int;
	ret->v.v_int=event.motion.xrel;
}

_fun(event_MouMo_yrel)
{
	ret->mode=type_int;
	ret->v.v_int=event.motion.yrel;
}

_fun(event_MouBu_which)
{
	ret->mode=type_int;
	ret->v.v_int=event.button.which;
}

_fun(event_MouBu_button)
{
	ret->mode=type_int;
	ret->v.v_int=event.button.button;
}

_fun(event_MouBu_state)
{
	ret->mode=type_int;
	ret->v.v_int=event.button.state;
}

_fun(event_MouBu_x)
{
	ret->mode=type_int;
	ret->v.v_int=event.button.x;
}

_fun(event_MouBu_y)
{
	ret->mode=type_int;
	ret->v.v_int=event.button.y;
}

_fun(event_JoyAx_which)
{
	ret->mode=type_int;
	ret->v.v_int=event.jaxis.which;
}

_fun(event_JoyAx_axis)
{
	ret->mode=type_int;
	ret->v.v_int=event.jaxis.axis;
}

_fun(event_JoyAx_value)
{
	ret->mode=type_int;
	ret->v.v_int=event.jaxis.value;
}

_fun(event_JoyBa_which)
{
	ret->mode=type_int;
	ret->v.v_int=event.jball.which;
}

_fun(event_JoyBa_ball)
{
	ret->mode=type_int;
	ret->v.v_int=event.jball.ball;
}

_fun(event_JoyBa_xrel)
{
	ret->mode=type_int;
	ret->v.v_int=event.jball.xrel;
}

_fun(event_JoyBa_yrel)
{
	ret->mode=type_int;
	ret->v.v_int=event.jball.yrel;
}

_fun(event_JoyHa_which)
{
	ret->mode=type_int;
	ret->v.v_int=event.jhat.which;
}

_fun(event_JoyHa_hat)
{
	ret->mode=type_int;
	ret->v.v_int=event.jhat.hat;
}

_fun(event_JoyHa_value)
{
	ret->mode=type_int;
	ret->v.v_int=event.jhat.value;
}

_fun(event_JoyBu_which)
{
	ret->mode=type_int;
	ret->v.v_int=event.jbutton.which;
}

_fun(event_JoyBu_button)
{
	ret->mode=type_int;
	ret->v.v_int=event.jbutton.button;
}

_fun(event_JoyBu_state)
{
	ret->mode=type_int;
	ret->v.v_int=event.jbutton.state;
}

_fun(event_Res_w)
{
	ret->mode=type_int;
	ret->v.v_int=event.resize.w;
}

_fun(event_Res_h)
{
	ret->mode=type_int;
	ret->v.v_int=event.resize.h;
}

unsigned int event_init_mask;

void event_init_Active(void)
{
	if (event_init_mask&0x0001) return ;
	regfun2(_event,"Act_gain",event_Act_gain);
	regfun2(_event,"Act_state",event_Act_state);
	event_init_mask|=0x0001;
}

void event_init_Keyboard(void)
{
	if (event_init_mask&0x0002) return ;
	regfun2(_event,"Key_which",event_Key_which);
	regfun2(_event,"Key_state",event_Key_state);
	regfun2(_event,"Key_key",event_Key_key);
	regfun2(_event,"Key_mod",event_Key_mod);
	event_init_mask|=0x0002;
}

void event_init_MouseMotion(void)
{
	if (event_init_mask&0x0004) return ;
	regfun2(_event,"MMo_which",event_MouMo_which);
	regfun2(_event,"MMo_state",event_MouMo_state);
	regfun2(_event,"MMo_x",event_MouMo_x);
	regfun2(_event,"MMo_y",event_MouMo_y);
	regfun2(_event,"MMo_xrel",event_MouMo_xrel);
	regfun2(_event,"MMo_yrel",event_MouMo_yrel);
	event_init_mask|=0x0004;
}

void event_init_MouseButton(void)
{
	if (event_init_mask&0x0008) return ;
	regfun2(_event,"MBu_which",event_MouBu_which);
	regfun2(_event,"MBu_button",event_MouBu_button);
	regfun2(_event,"MBu_state",event_MouBu_state);
	regfun2(_event,"MBu_x",event_MouBu_x);
	regfun2(_event,"MBu_y",event_MouBu_y);
	event_init_mask|=0x0008;
}

void event_init_JoyAxis(void)
{
	if (event_init_mask&0x0010) return ;
	regfun2(_event,"JAx_which",event_JoyAx_which);
	regfun2(_event,"JAx_axis",event_JoyAx_axis);
	regfun2(_event,"JAx_value",event_JoyAx_value);
	event_init_mask|=0x0010;
}

void event_init_JoyBall(void)
{
	if (event_init_mask&0x0020) return ;
	regfun2(_event,"JBa_which",event_JoyBa_which);
	regfun2(_event,"JBa_ball",event_JoyBa_ball);
	regfun2(_event,"JBa_xrel",event_JoyBa_xrel);
	regfun2(_event,"JBa_yrel",event_JoyBa_yrel);
	event_init_mask|=0x0020;
}

void event_init_JoyHat(void)
{
	if (event_init_mask&0x0040) return ;
	regfun2(_event,"JHa_which",event_JoyHa_which);
	regfun2(_event,"JHa_hat",event_JoyHa_hat);
	regfun2(_event,"JHa_value",event_JoyHa_value);
	event_init_mask|=0x0040;
}

void event_init_JoyButton(void)
{
	if (event_init_mask&0x0080) return ;
	regfun2(_event,"JBu_which",event_JoyBu_which);
	regfun2(_event,"JBu_button",event_JoyBu_button);
	regfun2(_event,"JBu_state",event_JoyBu_state);
	event_init_mask|=0x0080;
}

void event_init_Resize(void)
{
	if (event_init_mask&0x0100) return ;
	regfun2(_event,"Res_w",event_Res_w);
	regfun2(_event,"Res_h",event_Res_h);
	event_init_mask|=0x0100;
}

_fun(event_init)
{
	char *name;
	ret->mode=type_void;
	ret->v.v_void=NULL;
	if (n==0) return ;
	while(vlist->l) vlist=vlist->l;
	for(;vlist;vlist=vlist->r)
	{
		name=_string(vlist);
		if (!name) continue;
		if (strcmp(name,"All")==0)
		{
			event_init_Active();
			event_init_Keyboard();
			event_init_MouseMotion();
			event_init_MouseButton();
			event_init_JoyAxis();
			event_init_JoyBall();
			event_init_JoyHat();
			event_init_JoyButton();
			event_init_Resize();
		}
		else if (strcmp(name,"Active")==0) event_init_Active();
		else if (strcmp(name,"Keyboard")==0) event_init_Keyboard();
		else if (strcmp(name,"Mouse")==0)
		{
			event_init_MouseMotion();
			event_init_MouseButton();
		}
		else if (strcmp(name,"Joy")==0)
		{
			event_init_JoyAxis();
			event_init_JoyBall();
			event_init_JoyHat();
			event_init_JoyButton();
		}
		else if (strcmp(name,"Resize")==0) event_init_Resize();
		else if (strcmp(name,"MouseMotion")==0) event_init_MouseMotion();
		else if (strcmp(name,"MouseButton")==0) event_init_MouseButton();
		else if (strcmp(name,"JoyAxis")==0) event_init_JoyAxis();
		else if (strcmp(name,"JoyBall")==0) event_init_JoyBall();
		else if (strcmp(name,"JoyHat")==0) event_init_JoyHat();
		else if (strcmp(name,"JoyButton")==0) event_init_JoyButton();
		else dp("[%s].event.init: 未识别 event 类 %s\n",model_name,name);
	}
}

int model_init(void)
{
	int err;
	screen=NULL;
	w=0;
	h=0;
	loop=0;
	vbmp=NULL;
	event_init_mask=0;
	fps_thread=NULL;
	memset(&work,0,sizeof(Work));
	if (SDL_Init(SDL_INIT_VIDEO)<0)
	{
		dp("[%s]: 初始化 SDL 模块 fail\n",model_name);
		return -1;
	}
	KeyState=SDL_GetKeyState(&KeyNum);
	err|=regfun("model_name",model_name);
	err|=regfun("open",SDL_open);
	err|=regfun("close",SDL_close);
	err|=regfun("delay",SDL_delay);
	err|=regfun("update",SDL_update);
	err|=regfun("set_fps",set_fps);
	err|=regfun("get_bmp",get_bmp);
	err|=regfun("set_event",set_event);
	err|=regfun("wait_event",wait_event);
	err|=regfun("poll_event",poll_event);
	err|=regfun("in_loop",in_loop);
	err|=regfun("out_loop",out_loop);
	err|=regfun("set_title",set_title);
	err|=regfun("get_key",get_key);
	if (err) goto err;
	fps_var=fun->var_alloc(_vl->v.v_object,"fps",type_float|auth_noset|auth_norev,NULL);
	if (!fps_var) goto err;
	fps_var->v.v_float=0;
	_event=fun->var_alloc(_vl->v.v_object,"event",type_object,NULL);
	if (!_event) goto err;
	err|=regfun2(_event,"type",event_type);
	err|=regfun2(_event,"init",event_init);
	if (err) goto err;
	return 0;
	err:
	if (err) dp("[%s]: 初始化模块函数失败\n",model_name);
	return err;
}

void model_uini(void)
{
	if (work.noevent) free(work.noevent);
	if (work.activeevent) free(work.activeevent);
	if (work.keydown) free(work.keydown);
	if (work.keyup) free(work.keyup);
	if (work.mousemotion) free(work.mousemotion);
	if (work.mousebuttondown) free(work.mousebuttondown);
	if (work.mousebuttonup) free(work.mousebuttonup);
	if (work.joyaxismotion) free(work.joyaxismotion);
	if (work.joyballmotion) free(work.joyballmotion);
	if (work.joyhatmotion) free(work.joyhatmotion);
	if (work.joybuttondown) free(work.joybuttondown);
	if (work.joybuttonup) free(work.joybuttonup);
	if (work.quit) free(work.quit);
	if (work.syswmevent) free(work.syswmevent);
	if (work.videoresize) free(work.videoresize);
	if (work.videoexpose) free(work.videoexpose);
	if (vbmp)
	{
		vbmp->mode&=(~auth_noset)&(~auth_norev);
		fun->var_free(vbmp->v.v_object,"Type");
		fun->var_free(vbmp->v.v_object,"_data");
		fun->var_free(vbmp->v.v_object,"Width");
		fun->var_free(vbmp->v.v_object,"Height");
		vbmp=NULL;
	}
	if (fps_thread) SDL_KillThread(fps_thread);
	SDL_Quit();
	fps_thread=NULL;
	fps_var=NULL;
	KeyState=NULL;
	_event=NULL;
	screen=NULL;
}

