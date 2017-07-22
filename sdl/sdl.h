#ifndef _sdl_h_
#define _sdl_h_

#define ModelName "sdl-v1.0.0"

typedef struct S_work {
	char *noevent;		// 无事件（pop_event）
	char *activeevent;	// 应用程序 丢失/获得 可见性（即当前程序是否前台）
	char *keydown;		// 键按下
	char *keyup;		// 键松开
	char *mousemotion;	// 鼠标移动
	char *mousebuttondown;	// 鼠标按下
	char *mousebuttonup;	// 鼠标松开
	char *joyaxismotion;	// 操纵杆轴运动
	char *joyballmotion;	// 操纵杆轨迹球运动
	char *joyhatmotion;	// 操纵杆位置更改
	char *joybuttondown;	// 按下操纵杆按钮
	char *joybuttonup;	// 松开操纵杆按钮
	char *quit;		// 用户退出请求
	char *syswmevent;	// 系统特定事件
	char *videoresize;	// 用户调整视频格式大小
	char *videoexpose;	// 屏幕需要重绘
} Work;


#endif

