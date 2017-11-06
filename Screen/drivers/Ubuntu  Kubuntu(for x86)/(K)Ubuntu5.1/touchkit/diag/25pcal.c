/* tab: 4 */

/*
 * 25-points calibration
 *
 * syntax: prgname device_name
 *
 */

// #define MONITOR

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <stdio.h>
#include <signal.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/time.h>

#include "TouchCmd.h"
#include "tpanel.h"
#include "eeprom.h"
#include "correct.c"


#ifdef MONITOR
	#define M_Output(args...)								\
	{	char buf[1000];										\
		sprintf(buf, args);									\
		fputs(buf, Output);									\
	}														
#else
	#define M_Output(args...)
#endif
int				JobDone			= 0;
int				PtsTouched		= 0;
int 			DataEnough		= 0;
int				deviceFD;
char			*deviceName;
int				MARK_POINT[]	= _25P_MARK_POINT;
#define			_N				5
int				delta[_N*_N][2];				/* (dx, dy) dup 25 times */
int				PtsValue[_N*_N][2];

int				PtsSample[_N*_N][2] =
				{{154, 135}, {148, 522}, {163, 993}, {156, 1479}, {157, 1878},
				 {542, 138}, {543, 521}, {542, 990}, {545, 1472}, {548, 1875},
				 {1012, 144}, {1012, 520}, {1003, 1000}, {1009, 1477}, {1009, 1870},
				 {1468, 139}, {1459, 524}, {1469, 989}, {1462, 1477}, {1458, 1874},
				 {1854, 143}, {1842, 529}, {1849, 998}, {1841, 1485}, {1850, 1878}};

int				bitValid		= 11;
int				PanelTouched	= 0;

short dftXY[] = 
{ 0x66, 0x66, 0x799, 0x66, 0x66, 0x799, 0x799, 0x799 };

EEPROM			eeprom;

enum { _UL = 0, _UR = 4, _LL = 20, _LR = 24, _Center = 12 };
#define _X_		0
#define _Y_		1

#define			MaxErr			250

#define 		FONT_NAME		"9x15"
Display 		*display;
int				screen_num;
GC				gc;
Window			root;
Window			win;
XFontStruct 	*font_info;
unsigned int 	width, height;	/* window size */
char			*progname;
char			*RomFile;

int				bitinXY = 11;

#include		"diagcfg.h"
#include		"eeprom.c"
#include		"driver.c"
#include		"device.c"

void load_font(XFontStruct **font_info)
{
	char *fontname = FONT_NAME;

	if ((*font_info = XLoadQueryFont(display,fontname)) == NULL)
	{	printf( "Cannot open %s font\n", FONT_NAME);
		exit(1);
	}
}


void DrawCross(int x, int y, int width, int size, unsigned long color)
{	XSetForeground(display, gc, color);
	XSetLineAttributes(display, gc, /*line_width*/width, LineSolid, 
								CapRound, JoinRound);
	XDrawLine(display, win, gc, x-size, y, x+size, y);
	XDrawLine(display, win, gc, x, y-size, x, y+size);
}

void CrossBlink(unsigned long color)
{	int i, j;
	int cx, cy;
//	static int flag = 0;
	static int shift = 0;

	if (!DataEnough)
	{	int RectDist = width/200;
		i = PtsTouched / _N;
		j = PtsTouched % _N;
		cx = ( MARK_POINT[j] * width ) / 0x800;
		cy = ( MARK_POINT[i] * height ) / 0x800;

		#define NumRect		5
		XSetLineAttributes(display, gc, /*line_width*/1, LineSolid, 
									CapRound, JoinRound);
		for ( i=0; i<NumRect; i++)
		{	if ( (i+shift) % NumRect == 0 )
				XSetForeground(display, gc, BlinkCrossBG);
			else
				XSetForeground(display, gc, BlinkCrossFG);
			#ifdef ROUND_SYMBOL
				XDrawArc(display, win, gc,
								cx-i*RectDist, cy-i*RectDist,
								i*(2*RectDist), i*(2*RectDist), 0, 359*64);
			#else
				XDrawRectangle(display, win, gc,
									cx-i*RectDist, cy-i*RectDist,
									i*(2*RectDist), i*(2*RectDist));
			#endif
		}
		#undef NumRect
		shift++;
	}
}

void DrawText()
{	static char *prompt[] = {
		"                    25-Pt Calibration",
		"Pls Touch the Blinking Symbol until Beep or Stop Blinking",
		"                     (ESC to Abort)",
		};
	#define num			(sizeof prompt/sizeof prompt[0])
	static int init = 0;
	static int p_len[num];
	static int p_width[num];
	static int p_height;
	static int p_maxwidth=0;
	int i, x, y;
	int line_height;
	
	if (!init)
	{	for (i=0; i<num; i++)
		{	p_len[i] = strlen(prompt[i]);
			p_width[i] = XTextWidth(font_info, prompt[i], p_len[i]);
			if (p_width[i] > p_maxwidth)
			{	p_maxwidth = p_width[i];
			}
		}
		p_height = font_info->ascent + font_info->descent;
		init = 1;
	}
	line_height = p_height+5;	
	x = (width-p_maxwidth)/2;
	y = height / 2 - 6 * line_height;

	XSetForeground(display, gc, PromptText);	
	XClearArea(display, win, x-11, y-8-p_height, 
						p_maxwidth+11*2, num*line_height+8*2, False);
	XSetLineAttributes(display, gc, /*line_width*/3, FillSolid, 
								CapRound, JoinRound);
	XDrawRectangle(display, win, gc, x-11, y-8-p_height, 
						p_maxwidth+11*2, num*line_height+8*2);
	for (i=0; i<num; i++)
	{	XDrawString(display, win, gc, x, y+i*line_height, prompt[i], p_len[i]);
	}
	#undef num
}


void DrawGraphics()
{	int i, j;
	unsigned cx, cy;
	unsigned long color;
	
	DrawText();

	for (i=0; i<5; i++)
	{	for (j=0; j<5; j++)
		{	int num = 5*i + j;
		
			if (num == PtsTouched)
			{	continue;
			}else if (num > PtsTouched)
				color = nTouchedCross;
			else
				color = TouchedCross;

			cx = ( MARK_POINT[j] * width ) / 0x800;
			cy = ( MARK_POINT[i] * height ) / 0x800;
			DrawCross(cx, cy, width/200, width/64, color);
		}	
	}	
}


void getGC(Window win, GC *gc, XFontStruct *font_info)
{	unsigned long valuemask = 0; /* ignore XGCvalues and use defaults */
	XGCValues values;
	unsigned int line_width = 5;
//	int line_style = LineOnOffDash;
	int line_style = LineSolid;
	int cap_style = CapRound;
	int join_style = JoinRound;
//	int dash_offset = 0;
//	static char dash_list[] = {12, 24};
//	int list_length = 2;

	*gc = XCreateGC(display, win, valuemask, &values);

	XSetFont(display, *gc, font_info->fid);

	XSetLineAttributes(display, *gc, line_width, line_style, 
			cap_style, join_style);

//	XSetDashes(display, *gc, dash_offset, dash_list, list_length);
}


int getColor()
{	int			default_depth;
	Colormap	default_cmap;
	XColor		my_color;
	int			i;

	default_depth	= DefaultDepth(display, screen_num);	
	default_cmap	= DefaultColormap(display, screen_num);

	for (i=0; i<N_Colors; i++)
	{	XParseColor(display, default_cmap, colors[i], &my_color);
		XAllocColor(display, default_cmap, &my_color);
		pixels[i] = my_color.pixel;
	}
	return 0;
}


Cursor CreateEmptyCursor()
{	char nothing[] = {0};
	XColor nullcolor;
	Pixmap src = XCreateBitmapFromData(display, root, nothing, 1, 1);
	Pixmap msk = XCreateBitmapFromData(display, root, nothing, 1, 1);
	Cursor mycursor = XCreatePixmapCursor(display, src, msk, 
                               &nullcolor, &nullcolor, 0, 0);
	XFreePixmap(display, src);
	XFreePixmap(display, msk);
	return mycursor;
}

void sendCommand(int fd, char code)
{	char buf[] = "\x01_\x0d";
	
	buf[1] = code;
	write(fd, buf, 3);
}

void ProcessEvent()
{	XEvent event;

	while (XCheckWindowEvent(display, win, /*full mask&*/-1, &event) == True)
	{	switch  (event.type)
		{
		case KeyPress:
			{
			KeySym keysym = XKeycodeToKeysym(display, event.xkey.keycode, 0);
			
			if (keysym == XK_Escape && !PanelTouched)
			{	puts("Aborted");
				close(deviceFD);
				SYS_1(XUnloadFont(display, font_info->fid));
				XUngrabServer(display);
				XUngrabKeyboard(display, CurrentTime);
				SYS_1(XFreeGC(display, gc));
				SYS_0(XCloseDisplay(display));
				exit(0);
			}
			break;
			}
		case Expose:
			DrawGraphics(win, gc, width, height);
			break;
		}
	}
}


int setBaudRate(int dev, int whichbaud)
{	struct termios tty;

	tcgetattr(dev, &tty);
	tty.c_iflag = IGNBRK | IGNPAR;
	tty.c_oflag = 0;
	tty.c_lflag = 0;
	tty.c_line = 0;
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 1;
	tty.c_cflag = CS8|CREAD|CLOCAL|HUPCL;
	switch (whichbaud)
	{
	case 19200: whichbaud = B19200; break;
	case 9600:  whichbaud = B9600;  break;
	case 4800:  whichbaud = B4800;  break;
	case 2400:  whichbaud = B2400;  break;
	default:    whichbaud = B9600;  break;  /* default is 9600 :p */
	}
	tty.c_cflag |= whichbaud;
	tcsetattr(dev, TCSAFLUSH, &tty);
	return 0;  
}
                
                

#define DUMP(str,len)									\
{	int _i_;											\
	for (_i_=0; _i_<(len); _i_++)						\
		printf("%02X ", (unsigned char)(str)[_i_]);		\
}


/*
** (TakeAverage() will put the result in data[0].)
*/
#define TakeAverage(data, count) 					\
		/* int data[][2], count */					\
{	int _i;											\
	int _sX = 0, _sY = 0;							\
													\
	for (_i=0; _i<(count); _i++)					\
	{	_sX += (data)[_i][0];						\
		_sY += (data)[_i][1];						\
	}												\
	(data)[0][0] = _sX/count;						\
	(data)[0][1] = _sY/count;						\
}


#define SampleNumMAX	100

void ProcessFrameNew(char *frame)
{	
	int dx, dy, rx, ry;
	static int old_x, old_y;
	static int sampling = 1;
	static int sampleCount = 0;
	static int samples[SampleNumMAX][2];

	PanelTouched = Touched(frame[0]);

	if (!Touched(frame[0]))
	{	sampling = 1;
		sampleCount = 0;
		return;
	}
	if (!sampling)
	{	return;
	}

	bitValid = BitinXY(*frame);
	
	rx = X(frame);
	ry = Y(frame);

	dx = rx - old_x;
	dy = ry - old_y;

//	printf("dx, dy (%d, %d) === %d\n", dx, dy, sampleCount);
//	printf("rx, ry (%d, %d) === %d\n", rx, ry, sampleCount);

	old_x = rx;
	old_y = ry;

	#define DxMax	40
	#define DyMax	DxMax

#if 0 /* { */
	if (PtsTouched==0)	/* show hint */
	{	int cx, cy;
		cx = ( rx * width ) / 0x800;
		cy = height - ( ry * height ) / 0x800;
		XDrawArc(display, win, gc, cx-3, cy-3, 6, 6, 0, 359*64);
	}
#endif /* } */	
	
	if (-DxMax < dx && dx < DxMax 
		&&
		-DyMax < dy && dy < DyMax)
	{	samples[sampleCount][0] = rx;
		samples[sampleCount][1] = ry;
		sampleCount++;
		if ( sampleCount == SampleNumMAX )
		{	TakeAverage(samples, SampleNumMAX);
			PtsValue[PtsTouched][0] = samples[0][0];
			PtsValue[PtsTouched][1] = samples[0][1];
			#define FullVolumn 100
			XBell(display, FullVolumn);
			PtsTouched++;
			XClearWindow(display, win);
			DrawGraphics();
			XFlush(display);
			sampleCount = 0;
			sampling = 0;
		}
	}else
	{	sampleCount = 0;
	}
	#undef DxMax
	#undef DyMax
}

#define _FixToChar(dat)										\
	{	if ((dat)>0x7F)	(dat) = 0x7F;						\
		else if ((dat)<(-1L ^ 0x7F)) (dat) = (-1L ^ 0x7F );	\
	}

void AbsValueToDelta(Mapping *pMapping, int delta[][2])
{	int i;

	for (i=0; i<_N*_N; i++)
	{	int rx, ry, ex, ey;
		rx = PtsValue[i][_X_];
		ry = PtsValue[i][_Y_];
		CorrectPoint(pMapping, &rx, &ry);
		ex = (int)(MARK_POINT[i%_N]);
		ey = (int)(MAX_DIMENSION - MARK_POINT[i/_N]);
		delta[i][_X_] = rx - ex;
		delta[i][_Y_] = ry - ey;

		#ifdef ReportData
			printf( 
					"%2d:x(%4d,%4d,%4d),y(%4d,%4d,%4d)-",
					i, PtsValue[i][_X_], rx, ex, PtsValue[i][_Y_], ry, ey);
			if ( !((i+1)%2) ) printf("\n");
		#endif

		_FixToChar(delta[i][_X_]);
		_FixToChar(delta[i][_Y_]);
		
	}
}

#include <math.h>

typedef struct
{	int dx, dy;
}	Distance;

#define Macro_DistanceEqual(d1, d2)							\
	( ((d1)->dx*(d1)->dx + (d1)->dy*(d1)->dy) 				\
			== ((d2)->dx*(d2)->dx + (d2)->dy*(d2)->dy) )


#define Macro_MatchDistance(result, id1, id2, list, cnt)	\
{	int _i;													\
															\
	for (_i=cnt-1; _i>=0; _i--)								\
	{	Distance _d;										\
															\
		_d.dx = abs((id1)/_N - (id2)/_N);					\
		_d.dy = abs((id1)%_N - (id2)%_N);					\
		if ( Macro_DistanceEqual(&_d, (list)+_i) )			\
		{	break;											\
		}													\
	}														\
	(result) = _i;											\
}

void ChooseMapping(Mapping *mapping, int _25Raw[][2])
{
/*	const long w[5] = { 12, 8, 4, 2, 1 };*/
	const long w[5] = { 1, 0, 0, 0, 0};
	const long W = w[0] + w[1] + w[2] + w[3] + w[4];
	const struct
	{	long nume;
		long deno;
	}	ratio[5] = { {1, 1}, {9, 14}, {1, 1}, {9, 5}, {9, 5} };
	const Distance
		distance[5] = { {0, 0}, {1, 0}, {1, 1}, {2, 0}, {2, 1} };

	const int corner[4] = {_UL, _UR, _LL, _LR};
	int i, j;
	long result[4][2];
	int _25Vector[_N*_N][2];
	
	for (i=0; i<_N*_N; i++)
	{	_25Vector[i][_X_] = _25Raw[i][_X_] - _25Raw[_Center][_X_];
		_25Vector[i][_Y_] = _25Raw[i][_Y_] - _25Raw[_Center][_Y_];
	}
	
	for (i=0; i<4; i++)
	{	int cor = corner[i];
		result[i][_X_] = result[i][_Y_] = 0;
		for (j=0; j<_N*_N; j++)
		{	int match;
		
			Macro_MatchDistance(match, cor, j, distance, 5);
			if (match >= 0)
			{	result[i][_X_] += 
					_25Vector[j][_X_]*ratio[match].nume*w[match]
					/ratio[match].deno;
				result[i][_Y_] += 
					_25Vector[j][_Y_]*ratio[match].nume*w[match]
					/ratio[match].deno;
				#ifdef ReportData
					printf("%d:cor %d, idx %d, match %d   ", i, cor, j, match);
					printf("(%d,%d) * %ld * %ld / %ld  ",
						_25Vector[j][_X_], _25Vector[j][_Y_],
						ratio[match].nume, w[match], ratio[match].deno);
					printf("r(%ld, %ld)\n", result[i][_X_], result[i][_Y_]);
					getch();
				#endif
			}
		}
		result[i][_X_] /= W;
		result[i][_Y_] /= W;
	}
	
	mapping->uiULX = (int)(result[0][_X_] + _25Raw[_Center][_X_]);
	mapping->uiULY = (int)(result[0][_Y_] + _25Raw[_Center][_Y_]);
	mapping->uiURX = (int)(result[1][_X_] + _25Raw[_Center][_X_]);
	mapping->uiURY = (int)(result[1][_Y_] + _25Raw[_Center][_Y_]);
	mapping->uiLLX = (int)(result[2][_X_] + _25Raw[_Center][_X_]);
	mapping->uiLLY = (int)(result[2][_Y_] + _25Raw[_Center][_Y_]);
	mapping->uiLRX = (int)(result[3][_X_] + _25Raw[_Center][_X_]);
	mapping->uiLRY = (int)(result[3][_Y_] + _25Raw[_Center][_Y_]);
}

void SendEEPROM2Drv(char *dev, EEPROM rom)
{	int fd;
	char buf[500];
	char *wp;
	int i;

	if ( (fd = open(IDcat(PRJNAME, _CMDIN), O_RDWR)) < 0 )
	{	return;
	}
	sprintf(buf, "[%d]%s:", AP2D_SETEEPROM, dev);
	wp = strrchr(buf, ':') + 1;
	
	for (i=0; i<EEPROM_SIZE; i++)
	{	sprintf(wp, "%04X", (unsigned short)rom[i]);
		wp += 4;
	}
	*wp = '\0';
	write(fd, buf, strlen(buf));
	close(fd);
}


int Send25PtData()
{	int i, j;
	int rval = 0;
	
	/* send data to firmware */
	{	int corner[] = { _UL, _UR, _LL, _LR };
		int cnr2addr[4][2] = 
		{	{EPR_25ULX, EPR_25ULY},
			{EPR_25URX, EPR_25URY},
			{EPR_25LLX, EPR_25LLY},
			{EPR_25LRX, EPR_25LRY},
		};
		EEPROM backup;

		/* Backup */
		for (i=0; i<EEPROM_SIZE; i++)
		{	backup[i] = eeprom[i];
		}
		
		eeprom[EPR_Flags] |= EPR_F_25CoefOn;
		for (i=EPR_LLX; i<=EPR_URY; i++)
		{	eeprom[i] = dftXY[i-EPR_LLX];
		}
		for (i=0; i<4; i++)
		{	int cnr = corner[i];
			for (j=0; j<2; j++)
			{	eeprom[cnr2addr[i][j]] = (short)PtsValue[cnr][j];
			}
		}
		{	int delta[_N*_N][2];
			Mapping mapping;

			#ifdef ReportData
				printf("\n");
				for (i=0; i<_N*_N; i++)
				{	printf("  (%4d %4d)", PtsValue[i][_X_], PtsValue[i][_Y_]);
					if ( !((i+1)%_N) )
					{	printf("\n");
					}
				}
				getch();
			#endif
			
			ChooseMapping(&mapping, PtsValue);
			AbsValueToDelta(&mapping, delta);
			
			#ifdef ReportData				
				getch();
				printf("\n");
				for (i=0; i<_N*_N; i++)
				{	printf("  <%4d %4d>", delta[i][_X_], delta[i][_Y_]);
					if ( !((i+1)%_N) )
					{	printf("\n");
					}
				}
			#endif

			for (i=0; i<_N*_N; i++)
			{	int row, col;
			
				row = _N-1 - i/_N;
				col = i%_N;
				eeprom[EPR_25(col+row*_N+1)] =
					EPR_25MERGE(delta[i][_X_], delta[i][_Y_]);
			}
		}
		
		#define RETRY	5
		FixEEPROM(eeprom);
		for (i=0; i<RETRY; i++)
		{	if (!WriteEEPROM(deviceFD, eeprom))
			{	break;
			}
		}
		if (i>=RETRY)
		{	WriteEEPROM(deviceFD, backup);
			rval = 1;
		}else
		{	SendEEPROM2Drv(deviceName, eeprom);
			SaveEEPROM(RomFile, eeprom);
		}
		#undef RETRY
	}
	return rval;
}


double IdleTime = 0;
double Tick = 0;

void setTimer(double interval /* in second */)
{	struct itimerval timer;
	long sec = interval;
	long usec = (interval-sec) * 1.0e6;
	
	timer.it_value.tv_sec	= sec;
	timer.it_value.tv_usec	= usec;
	timer.it_interval = timer.it_value;
	setitimer(ITIMER_REAL, &timer, NULL);
	Tick = interval;
}

void memcpy_noPS2ack(char *dest, char *src, int *len)
{	int i;
	int orglen = *len;

	for (i=0; i<orglen; i++)
	{	if (src[i] == '\xFA' /*PS/2 Ack*/)
		{	(*len)--;
		}else
		{	*(dest++) = src[i];
		}
	}
}


void UpdateTimer(void)
{	int current = width*IdleTime/IDLETIMEOUT;

	XSetLineAttributes(display, gc, /*line_width*/2, LineSolid, 
						CapRound, JoinRound);
	XSetForeground(display, gc, Background);
	XDrawLine(display, win, gc, 0, height-1, current, height-1);
	XSetForeground(display, gc, TimerLine);
	XDrawLine(display, win, gc, current, height-1, width, height-1);
}

#ifdef MONITOR
FILE *Output;
#endif


int registerFASYNC(int fd, void (*handle)(int))
{	signal(SIGIO, handle);
	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | FASYNC);
	return 0;
}

void DrawMessage(char *msg)
{	char buf[300];
	char *prompt[] = { buf };
	#define num			(sizeof prompt/sizeof prompt[0])
	static int init = 0;
	static int p_len[num];
	static int p_width[num];
	static int p_height;
	static int p_maxwidth=0;
	int i, x, y;
	int line_height;
	
	strncpy(buf, msg, sizeof buf);	

	if ( 1 )
	{	for (i=0; i<num; i++)
		{	p_len[i] = strlen(prompt[i]);
			p_width[i] = XTextWidth(font_info, prompt[i], p_len[i]);
			if (p_width[i] > p_maxwidth)
			{	p_maxwidth = p_width[i];
			}
		}
		p_height = font_info->ascent + font_info->descent;
		init = 1;
	}
	line_height = p_height+5;	
	x = (width-p_maxwidth)/2;
	y = height / 2 - line_height;

	XSetForeground(display, gc, PromptText);	
	XSetLineAttributes(display, gc, /*line_width*/3, LineSolid, 
								CapRound, JoinRound);
	XClearArea(display, win, x-8, y-8-p_height, 
						p_maxwidth+8*2, num*line_height+8*2, False);
	XDrawRectangle(display, win, gc, x-8, y-8-p_height, 
						p_maxwidth+8*2, num*line_height+8*2);
	for (i=0; i<num; i++)
	{	XDrawString(display, win, gc, x, y+i*line_height, prompt[i], p_len[i]);
	}
	#undef num
}



void SigHandler(int num)
{	char buf[100];
	int i, rval;
	static int IamBusy = 0;
	static int frameSize = 0;	/* # of valid bytes in frame[] */
	static char frame[FrameSize];

	M_Output("25: I am busy %d. (%d)\n", IamBusy, PtsTouched);

	if ( IamBusy ) return;
	IamBusy = 1;
	
	if ( num == SIGALRM )
	{	if ( !JobDone)
		{	CrossBlink(BlinkCrossFG);
			UpdateTimer();
			if (IdleTime >= IDLETIMEOUT)
			{	puts("timeout");
				close(deviceFD);
				SYS_1(XUnloadFont(display, font_info->fid));
				XUngrabServer(display);
				XUngrabKeyboard(display, CurrentTime);
				SYS_1(XFreeGC(display, gc));
				SYS_0(XCloseDisplay(display));
				exit(0);
			}
			IdleTime += Tick;
			XFlush(display);
			ProcessEvent();
		}
	}else if ( num == SIGIO )
	{	rval = read(deviceFD, buf, 100);
		
		if (rval != -1)
		{	IdleTime = 0;
		}
		{	for (i=0; i<rval; i++)
			{	if ( IsSync(buf[i]) )
				{	frame[0] = buf[i];
					frameSize = 1;
				}else if ( frameSize )
				{	frame[frameSize++] = buf[i];
				}
				if ( frameSize == FrameSize )
				{	if (PtsTouched < _N*_N)
					{	ProcessFrameNew(frame);
						frameSize = 0;
						if ( PtsTouched == _N*_N )
						{	//setTimer(0);	/* stop timer */
							JobDone = 1;
						}
					}else
					{	PanelTouched = Touched(frame[0]);
					}
				}
			}
		}
	}

	M_Output("touched = %d\n", PanelTouched);	
	
	if (PtsTouched == _N*_N && !PanelTouched)
	{	DrawMessage("   Saving Data...   ");
		XFlush(display);
		if (Send25PtData())
		{	DrawMessage("   Saving Data...Failed   ");
			XFlush(display);
			sleep(1);
			puts("failed");
		}else
		{	DrawMessage("   Saving Data...OK   ");
			XFlush(display);
			sleep(1);
			puts("OK");
		}
		close(deviceFD);
		SYS_1(XUnloadFont(display, font_info->fid));
		XUngrabServer(display);
		XUngrabKeyboard(display, CurrentTime);
		SYS_1(XFreeGC(display, gc));
		SYS_0(XCloseDisplay(display));
		exit(0);
	}
	IamBusy = 0;
	return;
}



int split_atoi(char *src, char sep, int *rval, int cnt)
{	char *match;
	int done;

	for (done=0, match=src; match && *src && done<cnt; )
	{	char tmp;
	
		match = strchr(src, sep);
		if (match)
		{	tmp = *match;
			*match = '\0';
		}
		done += sscanf(src, "%d", rval+done);
		if (match)
		{	*match = tmp;
			src = match+1;
		}
	}
	return done;
}

int optZero = 0;
int optDebug = 0;
int optFile = 0;
char optFilename[300];

void GetOpt(int argc, char **argv)
{   char c;

	opterr = 1;
	while ( (c=getopt(argc, argv, "0df:")) != -1 )
	{	switch (c)
		{
		case '0':
			optZero = 1;
			return;
		case 'f':
			optFile = 1;
			strncpy(optFilename, optarg, sizeof optFilename);
			return;
		case 'd':
			optDebug = 1;
			return;
		}
	}
}

int Read25CoefFile(char *file)
{	int i, dat;
	FILE *fp;
	
	if ( (fp = fopen(file, "r")) == NULL )
	{	printf("failed to open\n");
		return 1;
	}
	for (i=0; i<25+8; i++)
	{	if ( feof(fp) )
		{	break;
		}
		if ( !fscanf(fp, "%d", &dat) )
		{	break;
		}
		if (i<25)
		{	eeprom[EPR_25(i+1)] = dat;
		}else
		{	eeprom[i + (EPR_25LLX-25)] = dat;
		}
	}
	fclose(fp);
	if ( i<25+8 )
	{	printf("data error\n");
		return 1;
	}
	for (i=EPR_LLX; i<=EPR_URY; i++)
	{	eeprom[i] = dftXY[i-EPR_LLX];
	}
	return 0;
}

void ProcessOption()
{	int i;
	EEPROM backup;
	
	for (i=0; i<EEPROM_SIZE; i++)
	{	backup[i] = eeprom[i];
	}
	
	if (optDebug)
	{	for (i=0; i<_N*_N; i++)
		{	PtsValue[i][0] = PtsSample[i][0];
			PtsValue[i][1] = PtsSample[i][1];
		}
		puts( Send25PtData() ? "Failed" : "OK" );
		return;
	} else if (optZero)
	{	int i;
		int changed = 0;
		
		for (i=1; i<=25; i++)
		{	changed |= eeprom[EPR_25(i)];
			eeprom[EPR_25(i)] = 0;
		}
		for (i=EPR_25LLX; i<=EPR_25URY; i++)
		{	if ( eeprom[EPR_Flags] & EPR_F_25CoefOn )
			{	changed |= eeprom[i - EPR_25LLX + EPR_LLX] != eeprom[i];
				eeprom[i - EPR_25LLX + EPR_LLX] = eeprom[i];
			}
			changed |= eeprom[i];
			eeprom[i] = 0;
		}
		changed |= eeprom[EPR_Flags] & EPR_F_25CoefOn;
		eeprom[EPR_Flags] &= ~EPR_F_25CoefOn;
		if (!changed)
		{	printf("OK\n");
			return;
		}
	}else if (optFile)
	{	if (Read25CoefFile(optFilename))
		{	return;
		}
		eeprom[EPR_Flags] |= EPR_F_25CoefOn;
	}
	#define RETRY	5
	FixEEPROM(eeprom);
	for (i=0; i<RETRY; i++)
	{	if (!WriteEEPROM(deviceFD, eeprom))
		{	break;
		}
	}
	if (i>=RETRY)
	{	WriteEEPROM(deviceFD, backup);
		printf("write error\n");
	}else
	{	SendEEPROM2Drv(deviceName, eeprom);
		printf("OK\n");
	}
	#undef RETRY
}

void SetRomFilename(char *name)
{	static char buf[100];
	char *slash;
	
	if ( (slash = strrchr(name, '/')) )
	{	name = slash + 1;
	}

	sprintf(buf, IDcat(PRJNAME, _ROMHOME) ROM_FORMAT, name);
	RomFile = buf;
}


int main(int argc, char **argv)
{	char *display_name = NULL;
	XSetWindowAttributes xswa;

	if ( argc == 1 )
	{	PRINT_25PCAL_HELP;
		return 0;
	}
	progname = argv[0];

	#ifdef MONITOR	
	{	Output = fopen("/dev/tty5", "a");
		if (!Output)
		{	puts("Can NOT open tty");
			return 0;
		}
	}
	#endif	

	StopDriver();

    if ( (deviceFD=open( argv[1], O_RDWR )) < 0 )
    {   printf("failed to open\n");
        return 0;
    }
    deviceName = argv[1];
    Dev_SaveState(deviceFD);
    Dev_SetBaud(deviceFD, 9600);

	SetRomFilename(deviceName);

    if (!LoopBackOK(deviceFD))
	{	printf("no responce\n");
		Dev_RestoreState(deviceFD);
		close(deviceFD);
		return 0;
	}
	
	if (CachedReadEEPROM(RomFile, deviceFD, eeprom))
	{	printf("bad eeprom\n");
		Dev_RestoreState(deviceFD);
		close(deviceFD);
		return 0;
	}

	GetOpt(argc-1, argv+1);
	
	if ( optZero || optFile || optDebug )
	{	ProcessOption();
		return 0;
	}

	
	/* connect to X server */
	if ( (display=XOpenDisplay(display_name)) == NULL )
	{	fprintf( stderr, "%s: cannot connect to X server %s\n", 
							progname, XDisplayName(display_name));
		close(deviceFD);
		exit(1);
	}

	screen_num = DefaultScreen(display);
	root = RootWindow(display, screen_num);
	
	/* setup window attributes */
	xswa.override_redirect = True;
	xswa.background_pixel = BlackPixel(display, screen_num);
	xswa.event_mask = ExposureMask | KeyPressMask;
	/* | KeyReleaseMask |
						ButtonPressMask | ButtonMotionMask;*/
	xswa.cursor = CreateEmptyCursor();
						
	/* get screen size from display structure macro */
	width = DisplayWidth(display, screen_num);
	height = DisplayHeight(display, screen_num);

	win = XCreateWindow(display, RootWindow(display,screen_num), 
				/*X*/ 0,  /*Y*/ 0, width, height, /*border*/ 0,
				CopyFromParent, InputOutput, CopyFromParent,
				CWOverrideRedirect | CWBackPixel | CWEventMask | CWCursor,
				&xswa);
	XMapWindow(display, win);
	XGrabKeyboard(display, win, False,
                       GrabModeAsync, GrabModeAsync, CurrentTime);
	XGrabServer(display);
	load_font(&font_info);
	getGC(win, &gc, font_info);
	getColor();

	XSetWindowBackground(display, win, Background);
	XClearWindow(display, win);

	signal(SIGALRM, SigHandler);
	setTimer(BLINKPERD);	
	registerFASYNC(deviceFD, SigHandler);
	
	while (1)
	{	pause();
	}
	return 0;	
}

