
/* tab: 4 */

/*
 * Full-Screen Draw Test
 *
 * syntax: prgname device_name [raw]
 *
 */
 

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <stdio.h>
#include <signal.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "TouchCmd.h"
#include "tpanel.h"
#include "eeprom.h"
#include "filter.h"
#include "filter.c"
#include "correct.c"

int				deviceFD;
int				MARK_POINT[]	= { 102, 511, 1023, 1535, 1944 };
int				bitValid = 11;
int				PanelTouched = 0;

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
int				flushing = 0;

#include		"diagcfg.h"
#include		"eeprom.c"
#include		"driver.c"
#include		"device.c"

EEPROM			eeprom;
Mapping			mapping;
LinearCoef		coef;
FILTERCOEFF		filter; 

int RawMode, _25Mode;

void load_font(XFontStruct **font_info)
{
	char *fontname = FONT_NAME;

	if ((*font_info = XLoadQueryFont(display,fontname)) == NULL)
	{	printf( "Cannot open %s font\n", FONT_NAME);
		close(deviceFD);
		exit(1);
	}
}

void DrawGraphics()
{	int i, j;
	unsigned cx, cy;

	XSetLineAttributes(display, gc, /*line_width*/1, LineSolid, 
						CapRound, JoinRound);
	XSetForeground(display, gc, DrawGrid);
	#define GRID	4
	for (i=0; i<=GRID; i++)
	{	cx = i*(width-1)/GRID;
		cy = i*(height-1)/GRID;
		XDrawLine(display, win, gc, cx, 0, cx, height);
		XDrawLine(display, win, gc, 0, cy, width, cy);
	}

	XSetLineAttributes(display, gc, /*line_width*/width/200, LineSolid, 
						CapRound, JoinRound);
	XSetForeground(display, gc, Cross);
	{	int SZ = width/64;
		for (i=0; i<5; i++)
		{	for (j=0; j<5; j++)
			{	cx = ( MARK_POINT[j] * width ) / 0x800;
				cy = ( MARK_POINT[i] * height ) / 0x800;
				XDrawLine(display, win, gc, cx-SZ, cy, cx+SZ, cy); 
				XDrawLine(display, win, gc, cx, cy-SZ, cx, cy+SZ); 
			}	
		}	
	}

	XSetLineAttributes(display, gc, /*line_width*/1, LineSolid, 
						CapRound, JoinRound);
}


void getGC(Window win, GC *gc, XFontStruct *font_info)
{	unsigned long valuemask = 0; /* ignore XGCvalues and use defaults */
	XGCValues values;
	*gc = XCreateGC(display, win, valuemask, &values);
	XSetFont(display, *gc, font_info->fid);
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

int registerFASYNC(int fd, void (*handle)(int))
{	signal(SIGIO, handle);
	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | FASYNC);
	return 0;
}

void ProcessFrame(char frame[])
{	static int touched = 0;
	static int old_cx, old_cy;
	int sz = width / 300;
	int x, y;
	int cx, cy;

	bitValid = BitinXY(*frame);	
	x = X(frame);
	y = Y(frame);
	PanelTouched = Touched(frame[0]);

	if (!RawMode)
	{	OnZTransfer(&x, &y, PanelTouched, &filter);
		if (_25Mode)
		{	CorrectPoint(&coef.mapping, &x, &y);
			Calibration25Pt(&coef, &x, &y);
		}
		CorrectPoint(&mapping, &x, &y);
	}


	cx = ( x * width ) / 0x800;
	cy = height - ( y * height ) / 0x800;

	XSetLineAttributes(display, gc, /*line_width*/2, FillSolid, 
								CapRound, JoinRound);
	if (!Touched(frame[0]))
	{	touched = 0;
		XSetForeground(display, gc, DrawLine);		
		XDrawLine(display, win, gc, cx, cy, old_cx, old_cy);
		XSetForeground(display, gc, DrawUp);
		XDrawArc(display, win, gc, cx-sz, cy-sz, 2*sz, 2*sz, 0, 359*64);
	} else
	{	if ( touched == 0 )
		{	XSetForeground(display, gc, DrawDown);
			XDrawLine(display, win, gc, cx-sz, cy-sz, cx+sz, cy+sz);
			XDrawLine(display, win, gc, cx+sz, cy-sz, cx-sz, cy+sz);
		} else 
		{	XSetForeground(display, gc, DrawLine);
			XDrawLine(display, win, gc, cx, cy, old_cx, old_cy);
		}
		old_cx = cx;
		old_cy = cy;
		touched = 1;		
	}
	XFlush(display);
}

void DrawText()
{	static char *prompt[] = {
		"     Draw Test",
		"SPACE: Clear Screen",
		"ESC:   Exit",
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

void DrawWarning()
{	static char *prompt[] = {
		"  Saving Data...  ",
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
	XClearArea(display, win, x-8, y-8-p_height, 
						p_maxwidth+8*2, num*line_height+8*2, False);
	XDrawRectangle(display, win, gc, x-8, y-8-p_height, 
						p_maxwidth+8*2, num*line_height+8*2);
	for (i=0; i<num; i++)
	{	XDrawString(display, win, gc, x, y+i*line_height, prompt[i], p_len[i]);
	}
	#undef num
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
			{	puts("OK");
				close(deviceFD);
				SYS_1(XUnloadFont(display, font_info->fid));
				SYS_1(XFreeGC(display, gc));
				SYS_0(XCloseDisplay(display));
				exit(0);
			}else if (keysym == '\x20') /* space */
			{	XClearWindow(display, win);
				DrawGraphics();
				XFlush(display);
			}
			break;
			}
		case Expose:
			DrawGraphics();
			DrawText();
			break;
		case SelectionNotify:
		}
	}
}

double IdleTime = 0;
double Tick = 0;

void UpdateTimer(void)
{	int current = width*IdleTime/IDLETIMEOUT;

	XSetLineAttributes(display, gc, /*line_width*/1, LineSolid, 
						CapRound, JoinRound);
	XSetForeground(display, gc, DrawGrid);
	XDrawLine(display, win, gc, 0, height-1, current, height-1);
	XSetForeground(display, gc, TimerLine);
	XDrawLine(display, win, gc, current, height-1, width, height-1);
}

void SigHandler(int num)
{	static int IamBusy = 0;
	static int cleared = 0;	
	static int touched = 0;
	
	if ( IamBusy ) return;
	IamBusy = 1;
	
	if ( num == SIGIO )
	{	static int frameSize = 0;	/* # of valid bytes in frame[] */
		static char frame[FrameSize];
		char buf[100];
		int i, rval;
		
		IdleTime = 0;
		cleared = 0;
		touched = 1;
		
		rval = read(deviceFD, buf, sizeof buf);

		for (i=0; i<rval; i++)
		{	if ( IsSync(buf[i]) )
			{	frame[0] = buf[i];
				frameSize = 1;
			}else if ( frameSize )
			{	frame[frameSize++] = buf[i];
			}
			if ( frameSize == FrameSize )
			{	ProcessFrame(frame);
			}
		}
	}else if ( num == SIGALRM )
	{	
		UpdateTimer();
		if (IdleTime >= IDLETIMEOUT)
		{	//puts("timeout");
			puts("OK");
			close(deviceFD);
			SYS_1(XUnloadFont(display, font_info->fid));
			SYS_1(XFreeGC(display, gc));
			SYS_0(XCloseDisplay(display));
			exit(0);
		}else if ( !cleared && touched && IdleTime * 4 > IDLETIMEOUT )
		{	XClearWindow(display, win);
			DrawGraphics();
			cleared = 1;
		}
		IdleTime += Tick;
		XFlush(display);
		ProcessEvent();
	}
	IamBusy = 0;
	return;
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

void SetRomFilename(char *name)
{	static char buf[100];
	char *slash;
	
	if ( (slash = strrchr(name, '/')) )
	{	name = slash + 1;
	}

	sprintf(buf, IDcat(PRJNAME, _ROMHOME) ROM_FORMAT, name);
	RomFile = buf;
}

void InitMyFilter(FILTERCOEFF *fp, char **argv)
{	InitFilterDefault(fp);

	for ( ; *argv; argv++ )
	{	int c[10];
		/* Coef0~3, C3Min, C3Max, Thold */
		if ( sscanf(*argv, "%d,%d,%d,%d,%d,%d,%d",
						c+0, c+1, c+2, c+3, c+4, c+5, c+6) == 7 )
		{	InitFilter(fp, c[0], c[1], c[2], c[3], c[4], c[5]);
			SetFilterThreshold(fp, c[6]);
		}
	}
}
                                                                            

int main(int argc, char **argv)
{	char *display_name = NULL;
	XSetWindowAttributes xswa;

	if ( argc == 1 )
	{	PRINT_DRAW_HELP;
		return 0;
	}
	
	InitMyFilter(&filter, argv);
	
	if ( argc > 2 && !strcasecmp(argv[2], "raw") )
	{	RawMode = 1;
	}else
	{	RawMode = 0;
	}
	progname = argv[0];

	StopDriver();
	
	if ( (deviceFD = open(argv[1], O_RDWR)) < 0 )
	{	printf("failed to open\n");
		return 0;
	}
    Dev_SaveState(deviceFD);
    Dev_SetBaud(deviceFD, 9600);

	SetRomFilename(argv[1]);

    if (!LoopBackOK(deviceFD))
	{	printf("no responce\n");
		Dev_RestoreState(deviceFD);
		close(deviceFD);
		return 0;
	}

	if (!RawMode)
	{	if (CachedReadEEPROM(RomFile, deviceFD, eeprom))
		{	printf("bad eeprom\n");
			Dev_RestoreState(deviceFD);
			close(deviceFD);
			return 0;
		}
		
		mapping.uiULX = eeprom[EPR_ULX];
		mapping.uiULY = eeprom[EPR_ULY];
		mapping.uiURX = eeprom[EPR_URX];
		mapping.uiURY = eeprom[EPR_URY];
		mapping.uiLLX = eeprom[EPR_LLX];
		mapping.uiLLY = eeprom[EPR_LLY];
		mapping.uiLRX = eeprom[EPR_LRX];
		mapping.uiLRY = eeprom[EPR_LRY];
		
		if ( (_25Mode = eeprom[EPR_Flags] & EPR_F_25CoefOn) )
		{	coef.mapping.uiULX = eeprom[EPR_25ULX];
			coef.mapping.uiULY = eeprom[EPR_25ULY];
			coef.mapping.uiURX = eeprom[EPR_25URX];
			coef.mapping.uiURY = eeprom[EPR_25URY];
			coef.mapping.uiLLX = eeprom[EPR_25LLX];
			coef.mapping.uiLLY = eeprom[EPR_25LLY];
			coef.mapping.uiLRX = eeprom[EPR_25LRX];
			coef.mapping.uiLRY = eeprom[EPR_25LRY];
			
			{	int col, row;
				for ( col=0; col<5; col++)
				{	for ( row=0; row<5; row++)
					{	short dat = eeprom[EPR_25(col + 5*row + 1)];
						coef.delta[0][col][row] = EPR_25X(dat);
						coef.delta[1][col][row] = EPR_25Y(dat);
					}
				}
			}                                    
		}
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

