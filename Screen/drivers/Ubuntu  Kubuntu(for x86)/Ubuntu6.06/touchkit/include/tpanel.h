/* tab size: 4 */

#ifndef _TPANEL_H_
#define _TPANEL_H_

#define _25PtCal

#include "config.h"
#include "TouchCmd.h"

typedef struct
{	int uiLLX, uiLLY;
	int uiLRX, uiLRY;
	int uiULX, uiULY;
	int uiURX, uiURY;
}	Mapping;

typedef struct
{	Mapping mapping;
	char delta[2][5][5];
}	LinearCoef;


#define TP_BUFSZ		100
#define TP_SHRBUFSZ		100
#define TP_MAX_CMD_SIZE		32

#include "eeprom.h"

typedef struct Panel
{	char *device;			/* name of device, e.g. /dev/ttyS0 */
	int fd;
	int len;				/* length (# of valid byte) of buf */
	char buf[TP_BUFSZ];
	/*
	 * from 0~1000, denote to which region of virtual panel this Panel maps
	 * in other word, full screen <==> (0,0), (1000,1000)
	 */ 
	enum { Normal, _4PCal, _25PCal, __anything_else__} state;
	int sharing;
	int present;
	int shrlen;
	char shrbuf[TP_SHRBUFSZ];
	int touched;
	int _25Mode;
	Mapping mapping;
	LinearCoef coef;
	EEPROM_UNIT chksum;
	struct Panel	*next;

	time_t		LastResetTime;	// Last Reset Time
	int		IdleResetInterval;	// reset interval in seconds
	int		DownResetInterval;	// Down reset interval in seconds
	
	char		chCmdBuffer[ TP_MAX_CMD_SIZE ];
	int		iCmdBytesRecved; 
	int		iCmdLen;	// Command Length
	int		bCmdPacket;	// Currently is Cmd packet;
	int		bPtPacket;	// currently is point packet
	int		bPtRecved;

}	TouchPanel;


/******************************
            misc...
******************************/

#define TP_NIY(something)

#define MAX(a,b)	((a)>(b)?(a):(b))
#define MIN(a,b)	((a)<(b)?(a):(b))

/*************************************************************************
                        device-host communication
*************************************************************************/

#define FrameSize	5

#ifdef DynamicalFrame
	#define BitinXY(frame)		((frame[0]&_ADRESOLUTION)+8)
#else 
	#define BitinXY(frame)		11
#endif

#define MAX_DIMENSION		((1<<BitinXY(0))-1)

#if defined(_SOH)
	#define cSOH	_SOH
#elif defined(_SOP)
	#define cSOH	_SOP
#else
	#define cSOH	0x01
#endif
#define cCR			_CR

/*#define sSOH	"\x01"*/
/*#define sCR		"\x0D"*/

#define _RCLICKBIT	0x20

/* int X(char dat[FrameSize]); */
#define X(dat)				( ((dat)[1] << 7) | (dat)[2] )
#define Y(dat)				( ((dat)[3] << 7) | (dat)[4] )

#define setX(dat, X)		( (dat)[1]=0x7f&((X)>>7),(dat)[2]=0x7f&(X) )
#define setY(dat, Y)		( (dat)[3]=0x7f&((Y)>>7),(dat)[4]=0x7f&(Y) )

#define setXY(dat, X, Y)	( setX(dat,X), setY(dat,Y) )
#define setRButton(frame)	((frame)[0] |= _RCLICKBIT)


/* bool IsSync(char c); */
#define IsSync(c)			((c)&_SYNCBIT)
#define Touched(c)			((c)&_DOWN)

#define IsRight(c)			((c)&_RCLICKBIT)
#define IsFirstByteOfFrame(c)   \
	(!((unsigned char)((c)^POINT_TOUCH) && (unsigned char)((c)^POINT_NOTOUCH)))

#define MaxCmdLen			20

/* void RemoveDataFrame(char*, int*) */
#define RemoveDataFrame(src, lenp)									\
{	int _i_, _d_;													\
	unsigned char *_buf_ = (src);									\
	for (_d_=_i_=0; _i_<*(lenp); _i_++)								\
	{	if (_buf_[_i_] == cSOH)										\
		{	if (_i_ == *(lenp)-1)									\
			{	_buf_[_d_++] = cSOH;								\
				break;												\
			}else if ( !_buf_[_i_+1] || _buf_[_i_+1] > MaxCmdLen )	\
			{	continue;											\
			}else 													\
			{	if (_d_ < _i_)										\
				{	memcpy(_buf_+_d_, _buf_+_i_, *(lenp) - _i_);	\
				}													\
				_d_ += *(lenp) - _i_;								\
				break;												\
			}														\
		}															\
	}																\
	*(lenp) = _d_;													\
}


/* void ExtractCmd(char*, int, int*, char*, int*) */
#define ExtractCmd(src, slen, cmdp, dest, dlenp)					\
{	int _i_;														\
	for (*(cmdp)=_i_=0; _i_<(slen); _i_++)							\
	{	if ( (src)[_i_] == cSOH )									\
		{	if ( _i_ == (slen) - 1 )								\
			{	break;												\
			}else if ( !(src)[_i_+1] || (src)[_i_+1] > MaxCmdLen )	\
			{	continue;											\
			}else if ( _i_ + (src)[_i_+1] + 1 < slen )				\
			{	*(cmdp) = (src)[_i_+2];								\
				*(dlenp) = (src)[_i_+1] - 1;						\
				memcpy((dest), (src)+_i_+3, *(dlenp));				\
				break;												\
			}else													\
			{	break;												\
			}														\
		}															\
	}																\
}



/* void FillCmd(char*, int, char*, int, int*); */

#define FillCmd(buf, cmd, dat, datlen, len_p)						\
{	int _i_;														\
	(buf)[0] = cSOH;												\
	(buf)[2] = (cmd);												\
	for (_i_=0; _i_ < (datlen); _i_++) (buf)[3+_i_] = (dat)[_i_];	\
	(buf)[1] = 1 + _i_;												\
	*(len_p) = _i_+3;												\
}

#define WriteFunc		write
#define SendCommand(fd, cmd, dat, datlen)					\
{	char _buf_[MaxCmdLen];									\
	int _len_;												\
	FillCmd(_buf_, (cmd), (dat), (datlen), &_len_);			\
	WriteFunc((fd), _buf_, _len_);							\
}

#define SendSimpleCommand(fd, cmd)		SendCommand((fd), (cmd), "", 0)


/*****************************************************
		Mouse Mode, Right Click :~
******************************************************/

#ifdef __MSDOS__
	#define MModeTag			"MouseMode"
	#define MapTag				"Mapping"
	#define PromptBeginTag		"PromptBegin"
	#define PromptEndTag		"PromptEnd"
	#define MaxTitle			800
#endif

#define STRLEN(constStr)	(sizeof(constStr)-1)	

typedef struct
{	enum {MM_NORMAL, MM_SELECT, MM_CLICK} mode;
	int selectDelay;
	int clickDelay;
	int inPlaceX;
	int inPlaceY;
	int thisButton;
	int nextButton;
	int touched;
	int rClickState;
	#ifdef __MSDOS__
	void far (*rClickAck)(void);
	#endif
}	MouseMode;

#ifdef __MSDOS__ /* { */

#define ReadBufSz	2000

typedef struct
{	int len;
	char data[ReadBufSz];
}	ReadBuf;

typedef struct
{	int reged;
	int nameid;
	ReadBuf far *fpBuf;
}	ExternReader;

typedef struct
{	int reged;
	char far *enable;
	char far *status;
	void (far *redraw)(void);
	char isDown;
}	RClickTool;

extern RClickTool RClick;


#define _LeftB		0
#define _MidB		1
#define _RightB		2

#define InPlaceMax				100
#define InPlaceMin				10
#define DelayMax				300
#define DelayMin				2

#define _DefaultInPlaceX		20
#define _DefaultInPlaceY		20
#define _DefaultClickDelay		30
#define _DefaultSelectDelay		30

#endif /* } */

#define DIAG_DONE				0
#define DIAG_NOANSWER			1
#define DIAG_BADGRAPH			2
#define DIAG_USRBREAK			3
#define DIAG_TIMEOUT			4
#define DIAG_WRFAILED			5

#define	_4P_MARK_POINT			{ 102, 2047-102 }
#define _25P_MARK_POINT			{ 102, 512, 1024, 2047-512, 2047-102 }

#define _merge(head,tail)		head##tail
#define IDcat(head,tail)		_merge(head,tail)

#define SYS_1( zzz... )                 \
{   while ( (zzz) != 1 );               \
}

#define SYS_0( zzz... )                 \
{   while ( (zzz) != 0 );               \
}

#endif /* _TPANEL_H_ */


