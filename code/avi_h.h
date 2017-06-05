#ifndef	_avi_h_h_
#define	_avi_h_h_

typedef struct __attribute__ ((packed)) S_avi_avih {
	char avih[4];		// 标识 avih 块，必须为 "avih"
	unsigned int size;	// 本数据结构的大小 =sizeof(Avi_avih)-8
	unsigned int msPF;	// 显示每桢所需的时间ms，定义avi的显示速率
	unsigned int maxBPS;	// 最大的数据传输率 ? fps*1000
	unsigned int PG;	// 填充粒度，记录块的长度需为此值的倍数，通常是2048 ?(0)
	unsigned int flags;	// AVI文件的特殊属性，如是否包含索引块，音视频数据是否交叉存储 ?(0x0910)
	unsigned int totalFs;	// 文件中的总桢数
	unsigned int initFs;	// 为交互格式指定初始帧数（非交互格式应该指定为0）
	unsigned int Streams;	// 本文件包含的流的个数 =1
	unsigned int sugBfsz;	// 建议使用的缓冲区的大小，通常为存储一桢图像以及同步声音所需要的数据之和
	unsigned int width;	// 图像宽
	unsigned int height;	// 图像高
	unsigned int res[4];	// 保留值
} Avi_avih;

typedef struct __attribute__ ((packed)) S_avi_strh {
	char strh[4];		// 必须为 "strh"
	unsigned int size;	// 本数据结构的大小 =sizeof(Avi_strh)-8
	char type[4];		// 流的类型: "auds"（音频流）、"vids"（视频流）、"mids"（MIDI流）、"txts"（文字流）
	char handler[4];	// 指定流的处理者，对于音视频来说就是解码器 ? =0
	unsigned int flags;	// 标记：是否允许这个流输出？调色板是否变化？ ?(0)
	unsigned short pri;	// 流的优先级（当有多个相同类型的流时优先级最高的为默认流） =0
	unsigned short lang;	// 流的语言 =0
	unsigned int initFs;	// 为交互格式指定初始帧数 =0
	unsigned int scale;	// 这个流使用的时间尺度 =1 (秒数)
	unsigned int rate;	// = fps (帧数)
	unsigned int start;	// 流的开始时间 =0
	unsigned int length;	// 流的长度（单位与 scale 和 rate 的定义有关） = totalFs
	unsigned int sugBfsz;	// 读取这个流数据建议使用的缓存大小
	unsigned int quality;	// 流数据的质量指标（0 ~ 10,000） =(~0)
	unsigned int SAMPsz;	// Sample(取样)的大小 ?(0)
	struct {
		short left;	// =0
		short top;	// =0
		short right;	// =width
		short bottom;	// =height
	} rcFrame;		// 指定这个流（视频流或文字流）在视频主窗口中的显示位置
				// 视频主窗口由AVIMAINHEADER结构中的dwWidth和dwHeight决定
} Avi_strh;

typedef struct __attribute__ ((packed)) S_avi_strf {
	char strf[4];		// 必须为 "strf"
	unsigned int size;	// 本数据结构的大小 =sizeof(Avi_strf)-8
	Bmp_info info;
} Avi_strf;

typedef struct __attribute__ ((packed)) S_avi_hi {
	// head
	char RIFF[4];
	unsigned int fsz;		// sizeof(Avi_hi)-8
	char AVI[4];
	// info-list hdrl
	char LIST_i[4];
	unsigned int LIST_i_sz;		// 16+sizeof(Avi_avih)+sizeof(Avi_strh)+sizeof(Bmp_info)
	char hdrl[4];
		// avih
		Avi_avih avih;		// sizeof(Avi_avih)
		// streams-list strl
		char LIST_s[4];
		unsigned int LIST_s_sz;	// 4+sizeof(Avi_strh)+sizeof(Avi_strf)
		char strl[4];
			// strh
			Avi_strh strh;	// sizeof(Avi_strh)
			// strf
			Avi_strf strf;	// sizeof(Bmp_info)
	// data-list movi
	char LIST_d[4];
	unsigned int LIST_d_sz;
	char movi[4];
} Avi_hi;

typedef struct S_avi {
	struct S_avi_hi hi;
	unsigned int width;
	unsigned int height;
	unsigned int fsz;
	unsigned int fps;
	unsigned int totalFs;
	unsigned int limitFs;
	unsigned int offset;
	FILE* fp;
} Avi;

Avi* avi_varget(var *, char *);
Avi* Avi_new(char *, signed int , signed int, unsigned int);

#endif

