
/* tab size: 4 */

#ifndef _DEVICE_C_
#define _DEVICE_C_

/*------------------------------------*\
 |   Basic Device (port) Operation    |
\*------------------------------------*/

//#define READ_DETAIL

#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#include "tpanel.h"

static struct termios TTYstate;

void Dev_SaveState(int dev)
{	tcgetattr(dev, &TTYstate);
}

void Dev_RestoreState(int dev)
{	tcsetattr(dev, TCSAFLUSH, &TTYstate);
}

int Dev_SetBaud(int dev, int whichbaud)
{	struct termios tty;

	tcgetattr(dev, &tty);
	tty.c_iflag = IGNBRK | IGNPAR;
	tty.c_oflag = 0;
	tty.c_lflag = 0;
	tty.c_line = 0;
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 0;
	tty.c_cflag = CS8|CREAD|CLOCAL;//|HUPCL;
	switch (whichbaud)
	{
	case 19200:	whichbaud = B19200;	break;
	case 9600:	whichbaud = B9600;	break;
	case 4800:	whichbaud = B4800;	break;
	case 2400:	whichbaud = B2400;	break;
	default:	whichbaud = B9600;	break;	/* default is 9600 :p */
	}
	tty.c_cflag |= whichbaud;
	tcsetattr(dev, TCSAFLUSH, &tty);
	return 0;
}

int ReadFunc(int fd, char *buf, int len)
{	fd_set fset;
	struct timeval tv;
	int rval;

	FD_ZERO(&fset);
	FD_SET(fd, &fset);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	if ( (rval = select( fd+1, &fset, NULL, NULL, &tv)) < 0 )
	{	rval = 0;
	}else if (rval)
	{	rval = read(fd, buf, len);
		if ( rval<0 && errno != ENODEV)
		{	perror("read");
		}
	}
	return rval;
}

void MilliSecDelay(int ms)
{	int p[2];
	fd_set fset;
	struct timeval tv;
	
	if ( ms && !pipe(p) )
	{	FD_ZERO(&fset);
		FD_SET(p[0], &fset);
		tv.tv_sec = ms / 1000;
		tv.tv_usec = (ms % 1000) * 1000;
		if ( select( p[0]+1, &fset, NULL, NULL, &tv) < 0 )
		{
		}
		close(p[0]);
		close(p[1]);
	}
}


int SendAndWait(int fd, int *cmd, char **cmdbuf, int *cmdlen)
{	fd_set fset;
	struct timeval tv;
	const double TIMEOUT_IN_SEC = 0.055;
	const int MaxDataIn = TIMEOUT_IN_SEC * 9600 / 8;
	const int Retry = 3;
	char Buf[2 * MaxDataIn];
	int BufDat = 0;

	FD_ZERO(&fset);
	FD_SET(fd, &fset);
	tv.tv_sec = ( int)( TIMEOUT_IN_SEC ); //0;
	if( TIMEOUT_IN_SEC > tv.tv_sec )
	{ 
		tv.tv_usec = ( TIMEOUT_IN_SEC - tv.tv_sec )* 1e6;
	}
	else
	{
		tv.tv_usec = TIMEOUT_IN_SEC * 1e6;
	}

	SendCommand(fd, *cmd, *cmdbuf, *cmdlen);
	if ( select( fd+1, &fset, NULL, NULL, &tv) <= 0 )
	{	return -__LINE__;
	}else
	{	int len, retry = Retry;
		
		while (1)
		{	if ( (len=ReadFunc(fd, Buf+BufDat, sizeof Buf-BufDat)) == 0 )
			{	MilliSecDelay( TIMEOUT_IN_SEC*1000/Retry);
				if (--retry == 0)
				{	return -__LINE__;
				}else
				{	continue;
				}
			}else if (len < 0)
			{	return -__LINE__;
			}
		#ifdef READ_DETAIL
			else
			{	int i;
				for ( i = 0; i < len; i++ )
				{	printf("[%02X]", (unsigned char)Buf[BufDat+i]);
				}
				puts("");
			}
		#endif
			BufDat += len;
			RemoveDataFrame(Buf, &BufDat);
			if (BufDat)
			{	retry = Retry;
				{	int _cmd = 0;
					static char _cmdbuf[MaxCmdLen];
					ExtractCmd(Buf, BufDat, &_cmd, _cmdbuf, cmdlen);
					if (_cmd)
					{	*cmd = _cmd;
						*cmdbuf = _cmdbuf;
						return 0;
					}
				}
			}
			if ( BufDat > MaxDataIn )
			{	return -__LINE__;
			}
		}
		return -__LINE__;
	}
	#undef TIMEOUT_IN_SEC
}

int LoopBackOK(int fd)
{	int cmd = H2D_LOOPBACK;
	char dummy[8] = "\0", *cmdbuf = dummy;
	int cmdlen = 0;
	return 1;

	// IF_IS_OTHER_SERIAL_DEVICE_THEN_RETURN_FALSE

	{	long flag;
		if ( !ioctl(fd, TIOCMGET, &flag)
			&& flag & (TIOCM_CTS | TIOCM_DSR) )
		{	return 0;
		}
	}


	// PS2 tolerance
	{	char buf[200];
		while ( ReadFunc(fd, buf, sizeof(buf)) ) {;}
		MilliSecDelay(50);
	}
	
	if ( !SendAndWait(fd, &cmd, &cmdbuf, &cmdlen) )
	{	if ( cmd == D2H_LOOPBACK ) return 1;
	}
	MilliSecDelay(50);
	if ( !SendAndWait(fd, &cmd, &cmdbuf, &cmdlen) )
	{	if ( cmd == D2H_LOOPBACK ) return 1;
	}
	MilliSecDelay(50);
	if ( !SendAndWait(fd, &cmd, &cmdbuf, &cmdlen) )
	{	return cmd == D2H_LOOPBACK;
	}

	#define USB_FW_BUG
	#ifdef USB_FW_BUG // {
	{	char pattern[100];
		char buf[200];
		int len;
		
		sprintf(pattern, "/proc/%d/fd/%d", getpid(), fd);
		if ( (len = readlink(pattern, buf, sizeof buf)) > 0 && len < sizeof buf )
		{	buf[len] = '\0';
			if ( !strncmp(buf, USBPORT, STRLEN(USBPORT)) )
			{	return 1;
			}
		}
	}
	#endif // }
	
	return 0;
}

#endif

