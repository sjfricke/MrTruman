
/*
 *******************************************************************************
 *******************************************************************************
 *
 * The code has been lifted from the Microtouch driver in xf86MuTouch.c
 * ($XFree86: xc/programs/Xserver/hw/xfree86/input/mutouch/xf86MuTouch.c,v 1.12 2000/06/07 22:03:10 tsi Exp $)
 *
 * Macbeth Chang, eGalax Inc., August 2001.
 *
 *******************************************************************************
 *******************************************************************************
 */

#include <xf86Version.h> 
#if XF86_VERSION_CURRENT >= XF86_VERSION_NUMERIC(3,9,0,0,0)
	#define XFREE86_V4
#endif

#ifdef XFREE86_V4 /* { */
#include <misc.h>
#include <xf86.h>

#if !defined(DGUX) 
	#include <xf86_ansic.h>
#endif

#include <xf86_OSproc.h>
#include <xf86Xinput.h>
#include <exevents.h>

#ifdef XFree86LOADER
	#include <xf86Module.h>
#endif

#else /* XFREE86_V4 */ /* } else { */

#include "Xos.h"
#include <signal.h>
#include <stdio.h>

#define	 NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "scrnintstr.h"
#include "XI.h"
#include "XIproto.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"
#include "xf86Xinput.h"
#include "xf86Version.h"

#include "os.h"
#include "osdep.h"
#include "exevents.h"

#include "extnsionst.h"
#include "extinit.h"

#endif /* XFREE86_V4 */ /* } */

/**********************************
 * Local Headers
 **********************************/

#include "tpanel.h" 
#include "TouchCmd.h"
#include "xf86TouchKit.h"

#define DRV_NAME		DRIVER_NAME

#define MaxNumPanel		20	/* we only support up to $MNP panels :p */
static struct
{	int num;
	TKPanelPrivatePtr list[MaxNumPanel];
}	gPanelList = {0};


#ifndef XFREE86_V4 /* { */
/*
 ***************************************************************************
 *
 * Configuration descriptor.
 *
 ***************************************************************************
 */

 

static SymTabRec TKTab[] = {
  { ENDSUBSECTION,     "endsubsection" },
  { PORT,              "port" },
  { DEVICENAME,        "devicename" },
  { SCREEN_NO,	       "screenno" },
  { MAPPING,           "mapping" },
  { DEBUG_LEVEL,       "debuglevel" },
  { LINK_SPEED,        "linkspeed" },
  { SELECT_DELAY,      "selectdelay" },
  { CLICK_DELAY,       "clickdelay" },
  { INPLACE,           "inplace" },
  { RCLICK_TOOL,       "rclicktool" },
  { ALWAYS_CORE,       "alwayscore" },    
  { -1,                "" },
};

#define LS300		1
#define LS1200		2
#define LS2400		3
#define LS9600		4
#define LS19200		5

static SymTabRec LinkSpeedTab[] = {
  { LS300,		"b300" },
  { LS1200,		"b1200" },
  { LS2400,		"b2400" },
  { LS9600,		"b9600" },
  { LS19200,	"b19200" }
};


typedef struct {
  int	speed;
} LinkParameterStruct;
  
static LinkParameterStruct	LinkSpeedValues[] = {
  { B300 },
  { B1200 },
  { B2400 },
  { B9600 },
  { B19200 }
};
#endif /* XFREE86_V4 */ /* } */


/*
 ***************************************************************************
 *
 * Default constants.
 *
 ***************************************************************************
 */
#define TK_MAX_TRIALS		5		/* Number of timeouts waiting for a	*/
									/* pending reply.			*/
#define TK_MAX_WAIT		300000	/* Max wait time for a reply (microsec) */
#define TK_LINK_SPEED		B9600	/* 9600 Bauds				*/
#define TK_PORT			"/dev/ttyS1"

/*
 ***************************************************************************
 *
 * Protocol constants.
 *
 ***************************************************************************
 */
					/* the leading and trailing bytes.		*/

#define TK_LEAD_BYTE		cSOH	/* First byte of a command/reply packet.	*/
#define TK_TRAIL_BYTE		cCR		/* Last byte of a command/reply packet.		*/


/*
 * Command reply values.
 */
#define TK_OK			'0'	/* Report success.				*/
#define TK_ERROR		'1'	/* Report error.				*/


/*
 ***************************************************************************
 *
 * Usefull macros.
 *
 ***************************************************************************
 */
#define SYSCALL(call)			while(((call) == -1) && (errno == EINTR))

/* This one is handy, thanx Fred ! */

#ifdef DBG
	#undef DBG
#endif

#ifdef DEBUG
	#undef DEBUG
#endif

static int      debug_level = 2;

#define DEBUG 1

#if DEBUG
	#define DBG(lvl, f) {if ((lvl) <= debug_level) f;}
#else
	#define DBG(lvl, f)
#endif

#ifdef XFREE86_V4 /* { */
	#undef SYSCALL
	#undef read
	#undef write
	#undef close
	#define SYSCALL(call) call
	#define read(fd, ptr, num) xf86ReadSerial(fd, ptr, num)
	#define write(fd, ptr, num) xf86WriteSerial(fd, ptr, num)
	#define close(fd) xf86CloseSerial(fd)
#endif /* } */


int g_bitinxy = 11;


#ifndef XFREE86_V4 /* { */
/*
 ***************************************************************************
 *
 * xf86TKConfig --
 *	Configure the driver from the configuration data.
 *
 ***************************************************************************
 */
static Bool
xf86TKConfig(LocalDevicePtr	*array,
			int					inx,
			int					max,
			LexPtr				val)
{
	LocalDevicePtr		local = array[inx];
	TKPanelPrivatePtr	priv = (TKPanelPrivatePtr)(local->private);
	int					token;
	
	while ((token = xf86GetToken(TKTab)) != ENDSUBSECTION)
	{	switch(token)
		{
		case PORT:
			if (xf86GetToken(NULL) != STRING)
				xf86ConfigError( DRV_NAME " input port expected");
			else 
			{	priv->input_dev = strdup(val->str);	
				if (xf86Verbose)
					ErrorF("%s " DRV_NAME " %s input port: %s\n",
							XCONFIG_GIVEN, local->name, priv->input_dev);
			}
			break;
		case DEVICENAME:
			if (xf86GetToken(NULL) != STRING)
				xf86ConfigError( DRV_NAME " device name expected");
			else
			{	local->name = strdup(val->str);
				if (xf86Verbose)
					ErrorF("%s " DRV_NAME " %s X device name: %s\n",
							XCONFIG_GIVEN, local->name, local->name);
			}
			break;
		case ALWAYS_CORE:
			xf86AlwaysCore(local, TRUE);
			if (xf86Verbose)
			{	ErrorF("%s TouchPanel device always stays core pointer\n",
				XCONFIG_GIVEN);
			}
			break;

		case DEBUG_LEVEL:
			if (xf86GetToken(NULL) != NUMBER)
			{	xf86ConfigError("Option number expected");
			}
			debug_level = val->num;
			if (xf86Verbose)
			{	
			#if DEBUG
				ErrorF("%s TouchPanel debug level sets to %d\n", XCONFIG_GIVEN,
						debug_level);
			#else
				ErrorF("%s TouchPanel debug level not sets to %d because"
						" debugging is not compiled\n", XCONFIG_GIVEN,
						debug_level);
			#endif
			}
		    break;			
		case EOF:
			FatalError("Unexpected EOF (missing EndSubSection)");
			break;

		default:
			xf86ConfigError( DRV_NAME " subsection keyword expected");
			break;
		}
	}

  DBG(2, ErrorF("xf86TKConfig port name=%s\n", priv->input_dev))
  return Success;
}
#endif /* XFREE86_V4 */ /* } */


/*
 ***************************************************************************
 *
 * xf86TKConvert --
 *	Convert extended valuators to x and y suitable for core motion
 *	events. Return True if ok and False if the requested conversion
 *	can't be done for the specified valuators.
 *
 ***************************************************************************
 */



static Bool
xf86TKConvert(LocalDevicePtr	local,
	       int		first,
	       int		num,
	       int		v0,
	       int		v1,
	       int		v2,
	       int		v3,
	       int		v4,
	       int		v5,
	       int		*x,
	       int		*y)
{	TKPanelPrivatePtr	priv = (TKPanelPrivatePtr) local->private;
	int					width = priv->screen_width;
	int					height = priv->screen_height;
	int					MaxMeasure = (1<<g_bitinxy)-1;

	if (first != 0 || num != 2)
		return FALSE;

  DBG(5, ErrorF("(1st %d) (n %d) (%d %d %d %d %d %d %d %d)\n",
  			first, num, v0, v1, v2, v3, v4, v5, *x, *y));

	*x = v0 * width / MaxMeasure;
	*y = v1 * height / MaxMeasure;
	*y = height - *y;

	if (*x < 0)			*x = 0;
	if (*x > width)		*x = width;
	if (*y < 0)			*y = 0;
	if (*y > height)	*y = height;	 

#ifdef XFREE86_V4 /* { */
  /*
   * Need to check if still on the correct screen.
   * This call is here so that this work can be done after
   * calib and before posting the event.
   */
  xf86XInputSetScreen(local, priv->screen_no, *x, *y);
#endif /* } */

  DBG(4, ErrorF("Convert: scr_no %d, (%d, %d) in (%d, %d)\n", 
  						priv->screen_no, *x, *y, width, height));

  return TRUE;
}



/*
 ***************************************************************************
 *
 * xf86TKReadInput --
 *	Read a buffer full of input from the touchscreen and enqueue
 *	all report packets found in it.
 *	If a packet is not fully received it is deferred until the next
 *	call to the function.
 *	Packet recognized by this function comply with the format :
 *
 *		Byte 1 :  Status flags with MSB set to 1
 *		Byte 2 :  X coordinate (lower bits)
 *		Byte 3 :  X coordinate (upper bits)
 *		Byte 4 :  Y coordinate (lower bits)
 *		Byte 5 :  Y coordinate (upper bits)
 *
 *	The routine can work with any of the two X device structs associated
 *	with the touchscreen. It is always possible to find the relevant
 *	informations and to emit the events for both devices if provided
 *	with one of the two structs. This point is relevant only if the
 *	two devices are actives at the same time.
 *

 ***************************************************************************
 */
static void
xf86TKReadInput(LocalDevicePtr	local)
{
  TKPanelPrivatePtr		priv = (TKPanelPrivatePtr)(local->private);
  int			cur_x, cur_y;
  int			state;
  int			num_bytes;
  int			bytes_in_packet;
  unsigned char		*ptr, *start_ptr;
  
  DBG(4, ErrorF("Entering ReadInput\n"));
  
  /*
   * Try to get a buffer full of report packets.
   */
  DBG(4, ErrorF("num_old_bytes is %d, Trying to read %d bytes from port\n",
		priv->num_old_bytes, TK_BUFFER_SIZE - priv->num_old_bytes));
  SYSCALL(num_bytes = read(local->fd,
			   (char *) (priv->rec_buf + priv->num_old_bytes),
			   TK_BUFFER_SIZE - priv->num_old_bytes));
  if (num_bytes < 0) {
    Error("System error while reading from " DRV_NAME " touchscreen.");
    return;
  }

  DBG(4, ErrorF("Read %d bytes of reports\n", num_bytes));
  num_bytes += priv->num_old_bytes;
  ptr = priv->rec_buf;
  bytes_in_packet = 0;
  start_ptr = ptr;

  while (num_bytes >= (FrameSize-bytes_in_packet))
  {
    /*
     * Skip bytes until a status byte (MSB set to 1).
     */
    if (bytes_in_packet == 0) {
      if ((ptr[0] & 0x80) == 0) {
	DBG(3, ErrorF("Dropping a byte in an attempt to synchronize a report packet: 0x%X\n",
		      ptr[0]));
	start_ptr++;
      }
      else {
	bytes_in_packet++;
      }
      num_bytes--;
      ptr++;
    }
    else if (bytes_in_packet != FrameSize) {
      if ((ptr[0] & 0x80) == 0) {
	bytes_in_packet++;
      }
      else {
	/*
	 * Reset the start of packet, we have most certainly
	 * lost some data.
	 */
	DBG(3, ErrorF("Reseting start of report packet data has been lost\n"));
	bytes_in_packet = 1;
	start_ptr = ptr;
      }
      ptr++;
      num_bytes--;
    }

	if (bytes_in_packet == FrameSize)
	{

      /*
       * First stick together the various pieces.
       */
      state = start_ptr[0];
      g_bitinxy = BitinXY(state);
      cur_x = X(start_ptr);
      cur_y = Y(start_ptr);
      
      DBG(3, ErrorF("Packet: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
		    start_ptr[0], start_ptr[1], start_ptr[2], start_ptr[3], start_ptr[4]));
      start_ptr = ptr;
      bytes_in_packet = 0;
      
      /*
       * Send events.
       *
       * We *must* generate a motion before a button change if pointer
       * location has changed as DIX assumes this. This is why we always
       * emit a motion, regardless of the kind of packet processed.
       *
       */
	/*
	 * Emit a motion. If in core pointer mode we need to calibrate
	 * or we will feed X with quite bogus event positions.
	 */

      DBG(3, ErrorF("Post Motion: %d %d %d\n", cur_x, cur_y, TRUE));
	 
	xf86PostMotionEvent(local->dev, TRUE, 0, 2, cur_x, cur_y);
	
	/*
	 * Emit a button press or release.
	 */
	 
	if ( Touched(state) != Touched(priv->state))
	{	static int fired = 0;
		
		DBG( 3, ErrorF("S(%02X) Post %c %s\n", 
						(unsigned char)state,
						IsRight(state) ? 'R' : 'L',
						Touched(state) ? "Down" : " Up"));
						
		xf86PostButtonEvent(local->dev, TRUE, 
					(IsRight(state) ? _RightButton : _LeftButton),
					Touched(state), 0, 2, cur_x, cur_y);
	/***********************************************************************
		@#$*!... *>_<*
		
		(Bug?!) X server ALWAYS treats our first button event as (Left-Down)
	***********************************************************************/
		if (!fired && IsRight(priv->state))
		{
			#define ButtonUp 0
			#define ButtonDn 1
			xf86PostButtonEvent(local->dev, TRUE, 
					_LeftButton, ButtonUp, 0, 2, cur_x, cur_y);
			xf86PostButtonEvent(local->dev, TRUE, 
					_RightButton, ButtonDn, 0, 2, cur_x, cur_y);
		}
		fired = 1;
	}
      DBG(3, ErrorF("TouchScreen: (%c) x(%d), y(%d), %s\n",
      		IsRight(priv->state) ? 'R' : 'L',
		    cur_x, cur_y, 
		    ( (Touched(state) != Touched(priv->state)) ?
		     ( Touched(state) ? "Press" : "Release") : "Stream")));
      priv->state = state;
    }
  }

  /*
   * If some bytes are left in the buffer, pack them at the
   * beginning for the next turn.
   */
  if (num_bytes != 0) {
    memcpy(priv->rec_buf, ptr, num_bytes);
    priv->num_old_bytes = num_bytes;
  }
  else {
    priv->num_old_bytes = 0;
  }
}

/*
 ***************************************************************************
 *
 * xf86TKSendPacket --
 *	Emit a variable length packet to the controller.
 *	The function expects a valid buffer containing the
 *	command to be sent to the controller.  The command
 *	size is in len
 *	The buffer is filled with the leading and trailing
 *	character before sending.
 *
 ***************************************************************************
 */
static Bool
xf86TKSendPacket(int cmd, int fd)
{	char packet[] = { cSOH, '?', cCR };
	int	result;

	packet[1] = cmd;

  DBG(4, ErrorF("Sending packet : 0x%X 0x%X 0x%X ( %c)\n",
  				packet[0], packet[1], packet[2], packet[1]));
  SYSCALL(result = write(fd, packet, sizeof packet));
  if (result != sizeof packet) {
    DBG(5, ErrorF("System error while sending to " DRV_NAME " touchscreen.\n"));
    return !Success;
  }
  else {
    return Success;
  }
}


/*
 ***************************************************************************
 *
 * xf86TKGetReply --
 *	Read a reply packet from the port. Synchronize with start and stop
 *	of packet.
 *      The packet structure read by this function is as follow:
 *		Byte 0 : TK_LEAD_BYTE
 *		Byte 1
 *		...
 *		Byte n : packet data
 *		Byte n+1 : TK_TRAIL_BYTE
 *
 *	This function returns if a valid packet has been assembled in
 *	buffer or if no more data is available to do so.
 *
 *	Returns Success if a packet is successfully assembled.
 *	Bytes preceding the TK_LEAD_BYTE are discarded.
 *	Returns !Success if out of data while reading. The start of the
 *	partially assembled packet is left in buffer, buffer_p reflects
 *	the current state of assembly. Buffer should at least have room
 *	for TK_BUFFER_SIZE bytes.
 *
 ***************************************************************************
 */
static Bool
xf86TKGetReply(unsigned char *buffer, int *buffer_p, int fd)
{
  int	num_bytes;

  DBG(4, ErrorF("Entering xf86TKGetReply with buffer_p == %d\n", *buffer_p));
  
  /*
   * Try to read enough bytes to fill up the packet buffer.
   */
  DBG(4, ErrorF("buffer_p is %d, Trying to read %d bytes from port\n",
		*buffer_p, TK_BUFFER_SIZE - *buffer_p));
  SYSCALL(num_bytes = read(fd,
			   (char *) (buffer + *buffer_p),
			   TK_BUFFER_SIZE - *buffer_p));
  
  /*
   * Okay, give up.
   */
  if (num_bytes < 0) {
    Error("System error while reading from " DRV_NAME " touchscreen.");
    return !Success;
  }
  DBG(4, ErrorF("Read %d bytes of reply\n", num_bytes));
    
  while (num_bytes) {
    /*
     * Sync with the start of a packet.
     */
    if ((*buffer_p == 0) && (buffer[0] != TK_LEAD_BYTE)) {
      /*
       * No match, shift data one byte toward the start of the buffer.
       */
      DBG(4, ErrorF("Dropping one byte in an attempt to synchronize: '%c' 0x%X\n", 
		    buffer[0], buffer[0]));
      memcpy(&buffer[0], &buffer[1], num_bytes-1);
      num_bytes--;
    }
    else if (buffer[*buffer_p] == TK_TRAIL_BYTE) {
      /*
       * Got a packet, report it.
       */
      *buffer_p = 0;
      return Success;
    }
    else {
      num_bytes--;
      (*buffer_p)++;
    }
  }
  return !Success;
}


/*
 ***************************************************************************
 *
 * xf86TKWaitReply --
 *	It is assumed that the reply will be in the few next bytes
 *	read and will be available very soon after the command post. if
 *	these two asumptions are not met, there are chances that the server
 *	will be stuck for a while.
 *	The reply is left in reply. The function returns Success if a valid
 *	reply was found and !Success otherwise. Reply should at least
 *	have room for TK_BUFFER_SIZE bytes.
 *
 ***************************************************************************
 */
#ifndef XFREE86_V4 /* { */
static int
xf86WaitForInput(int	fd,
		 int	timeout)
{
  fd_set	readfds;
  struct	timeval to;
  int		r;
  
  FD_ZERO(&readfds);
  FD_SET(fd, &readfds);
  to.tv_sec = 0;
  to.tv_usec = timeout;
  
  SYSCALL(r = select(FD_SETSIZE, &readfds, NULL, NULL, &to));
  return r;
}
#endif /* } */

static Bool
xf86TKWaitReply(unsigned char *reply, int fd)
{	Bool ok;
	int i, result;
	int reply_p = 0;
	unsigned char local_reply[30];
	
	DBG(4, ErrorF("Waiting a reply\n"));
	i = TK_MAX_TRIALS;
	do {
		ok = !Success;
    /*
     * Wait half a second for the reply. The fuse counts down each
     * timeout and each wrong packet.
     */
	
		DBG(4, ErrorF("Waiting %d ms for data from port\n", TK_MAX_WAIT / 1000));
		result = xf86WaitForInput(fd, TK_MAX_WAIT);
		if (result > 0) {
			if (reply) {
				ok = xf86TKGetReply(reply, &reply_p, fd);
			}else {
				ok = xf86TKGetReply(local_reply, &reply_p, fd);
				if (ok && local_reply[1] != TK_OK) {
					DBG(3, ErrorF("Error reported by firmware\n"));
					ok = !Success;
				}
			}
		}else {
			DBG(3, ErrorF("No answer from port : %d\n", result));
		}
		if (result == 0)
			i--;
	} while(ok != Success && i);
	return ok;
}


/*
 ***************************************************************************
 *
 * xf86TKSendCommand --
 *	Emit a command to the controller and blocks until the reply is
 *	read.
 *
 *	The reply is left in reply. The function returns Success if the
 *	reply is valid and !Success otherwise. Reply should at least
 *	have room for TK_BUFFER_SIZE bytes.
 *
 ***************************************************************************
 */
static Bool
xf86TKSendCommand(int cmd, unsigned char *reply, int fd)
{	Bool			ok;

	if (xf86TKSendPacket(cmd, fd) == Success)
	{	ok = xf86TKWaitReply(reply, fd);
		return ok;
	}else
	{	return !Success;
	}
}


static void SendRClickRequest(int status)
{	FILE *fp;
	if ( (fp=fopen(IDcat(PRJNAME, _CMDIN), "r+")) )
	{	fprintf(fp, "[%d]0\n",
					status ? AP2D_RCLICKDELAYON : AP2D_XMODULEREADY);
		fclose(fp);
	}
}

static void SendFlushRequest()
{	FILE *fp;
	if ( (fp=fopen(IDcat(PRJNAME, _CMDIN), "r+")) )
	{	fprintf(fp, "[%d]0\n", AP2D_FLUSHOUTPUT);
		fclose(fp);
	}
}

static int IsComment(char *line)
{
	for (; *line && isspace(*line); line++)
	{;}
	return (*line == '#');
}

#define ToUpper(string)								\
{	int _i_;										\
	for (_i_=0; (string)[_i_]; _i_++)				\
	{	(string)[_i_] = toupper((string)[_i_]);		\
	}												\
}

static int NeedRClickTool()
{	FILE *fp;
	char line[1000];
	char *equal;
	int rval = 0;
	char keyword[30] = _RClickTool;

	if ( (fp=fopen(IDcat(PRJNAME, _CONF), "r")) )
	{	ToUpper(keyword);
		while (fgets(line, sizeof line, fp))
		{	if ( !IsComment(line) && (equal=strchr(line, '=')) )
			{	*equal = '\0';
				ToUpper(line);
				if (strstr(line, keyword)) 
				{	sscanf(equal+1, "%d", &rval);
				}
			}
		}
		fclose(fp);
	}
	return rval;
}

/*
 ***************************************************************************
 *
 * xf86TKControl --
 *
 ***************************************************************************
 */
static Bool
xf86TKControl(DeviceIntPtr	dev,
	       int		mode)
{	LocalDevicePtr	local = (LocalDevicePtr) dev->public.devicePrivate;
	TKPanelPrivatePtr		priv = (TKPanelPrivatePtr)(local->private);
	#define NumButtons	4
	unsigned char		map[NumButtons+10] = { 0, 1, 2, 3, 4 };
	unsigned char		reply[TK_BUFFER_SIZE];
	char *id_string = DEVICE_ID(local->private_flags) == PANEL_ID ? "panel" : "control";
	
	switch(mode)
	{
	case DEVICE_INIT:
	{	DBG(2, ErrorF( DRV_NAME " %s init...\n", id_string));
		SendFlushRequest();
		
		if (priv->screen_no >= screenInfo.numScreens ||
			priv->screen_no < 0)
		{	priv->screen_no = 0;
		}
		priv->screen_width = screenInfo.screens[priv->screen_no]->width;
		priv->screen_height = screenInfo.screens[priv->screen_no]->height;
		
		/*
		 * Device reports button press for up to 3 buttons.
		 */
		if (InitButtonClassDeviceStruct(dev, NumButtons, map) == FALSE)
		{	ErrorF("Unable to allocate ButtonClassDeviceStruct\n");
			return !Success;
		}
		
      /*
       * Device reports motions on 2 axes in absolute coordinates.
       * Axes min and max values are reported in raw coordinates.
       * Resolution is computed roughly by the difference between
       * max and min values scaled from the approximate size of the
       * screen to fit one meter.
       */
      if (InitValuatorClassDeviceStruct(dev, 2, xf86GetMotionEvents,
					local->history_size, Absolute) == FALSE) {
	ErrorF("Unable to allocate ValuatorClassDeviceStruct\n");
	return !Success;
      }
      else {
	InitValuatorAxisStruct(dev, 0, 0, 4000,
			       9500,
			       0     /* min_res */,
			       9500  /* max_res */);
	InitValuatorAxisStruct(dev, 1, 0, 4000,
			       10500,
			       0     /* min_res */,
			       10500 /* max_res */);
      }
      
      if (InitFocusClassDeviceStruct(dev) == FALSE) {
	ErrorF("Unable to allocate FocusClassDeviceStruct\n");
      }
      
      /*
       * Allocate the motion events buffer.
       */
      xf86MotionHistoryAllocate(local);
      
      /*
       * This once has caused the server to crash after doing an xalloc & strcpy ??
       */
#ifndef XFREE86_V4 
      AssignTypeAndName(dev, local->atom, local->name);
#endif
      
      DBG(2, ErrorF("Done.\n"));
      return Success;
	}
    
	case DEVICE_ON:
	{	Bool	already_open = FALSE;
      
      DBG(2, ErrorF( DRV_NAME " %s on...\n", id_string));

      /*
       * Try to see if the port has already been opened either
       * for this device or for the other one.
       */
		
		if (local->fd >= 0)
		{	already_open = TRUE;
		} else
		{	if (priv->local && priv->local->fd >= 0)
			{	already_open = TRUE;
				local->fd = priv->local->fd;
			}
		}
		
		if (!already_open)
		{	
			DBG(2, ErrorF( DRV_NAME " touchscreen opening : %s\n", priv->input_dev));
#ifdef XFREE86_V4 /* { */
			local->fd = xf86OpenSerial(local->options);
			if (local->fd < 0)
			{	Error("Unable to open " DRV_NAME " touchscreen device");
				return !Success;
			}
#else /* } else { */
			SYSCALL(local->fd = open(priv->input_dev, O_RDWR|O_NDELAY, 0));
			if (local->fd < 0)
			{	Error("Unable to open " DRV_NAME " touchscreen device");
				return !Success;
			}
	/*
	 * Try to see if the link is at the specified rate and
	 * reset the controller. The wait time needed by the
	 * controller after reset should be compensated by the
	 * timeouts in the receive section.
	 */
#endif /* } */            

	if (xf86TKSendCommand(H2D_LOOPBACK, NULL, local->fd) != Success)
	{	DBG(3, ErrorF("Not at the specified rate, giving up\n"));
		goto not_success;
	}
	
	/*
	 * ask the controller to report identity and status.
	 */
	if (xf86TKSendCommand(H2D_GETMODEL, reply, local->fd) != Success)
	{	ErrorF("Unable to ask " DRV_NAME " touchscreen identification\n");
		goto not_success;
	}
	
	/*
	 * Set the operating mode: Format Tablet, Mode stream, Pen.
	 */

	AddEnabledDevice(local->fd);
      }

      dev->public.on = TRUE;  
    
      DBG(2, ErrorF("Done\n"));
      
      SendRClickRequest(NeedRClickTool());
      
      return Success;
      
    not_success:
      SYSCALL(close(local->fd));
      local->fd = -1;
      return !Success;
    }
  
  /*
   * Deactivate the device.
   */
  case DEVICE_OFF:
    DBG(2, ErrorF( DRV_NAME " %s off...\n", id_string));
    dev->public.on = FALSE;
    DBG(2, ErrorF("Done\n"));
    return Success;
    
    /*
     * Final close before server exit. This is used during server shutdown.
     * Close the port and free all the resources.
     */
  case DEVICE_CLOSE:
    DBG(2, ErrorF( DRV_NAME " %s close...\n", id_string));
    dev->public.on = FALSE;
    if (local->fd >= 0) {
#ifdef XFREE86_V4
      xf86RemoveEnabledDevice(local);
#else
      RemoveEnabledDevice(local->fd);
#endif
      SYSCALL(close(local->fd));
      local->fd = -1;
      /*
       * Need some care to close the port only once.
       */
	  if (priv->local) {
	    priv->local->fd = -1;
	  }
    }
    {	int i;
	    for (i=0; i<gPanelList.num; i++)
		{	if (gPanelList.list[i] == local->private)
			{	gPanelList.list[i] = gPanelList.list[--gPanelList.num];
				break;
			}
		}
	}
    DBG(2, ErrorF("Done\n"));  
    return Success;

  default:
      ErrorF("unsupported mode=%d\n", mode);
      return !Success;
	}
}


/*
 ***************************************************************************
 *
 * xf86TKAllocate --
 *
 ***************************************************************************
 */
static LocalDevicePtr
#ifndef XFREE86_V4 /* { */
xf86TKAllocate(char		*name,
		char		*type_name,
		int		flag)
#else /* } else { */
xf86TKAllocate(InputDriverPtr	drv,
		char		*name,
		char		*type_name,
		int		flag)
#endif /* } */
{
#ifndef XFREE86_V4
  LocalDevicePtr        local = (LocalDevicePtr) xalloc(sizeof(LocalDeviceRec));
#else
  LocalDevicePtr        local = xf86AllocateInput(drv, 0);
#endif

	if ( !strcmp(name, XI_PANEL) )
	{	TKPanelPrivatePtr         priv = (TKPanelPrivatePtr) xalloc(sizeof(TKPanelPrivateRec));
		if (!local) {
			if (priv) {
				xfree(priv);
			}
			return NULL;
		}
		if (!priv) {
			if (local) {
				xfree(local);
			}
			return NULL;
		}
		priv->input_dev = NULL;
		#ifndef XFREE86_V4
			priv->link_speed = TK_LINK_SPEED;
		#endif
		priv->screen_no = 0;
		priv->screen_width = -1;
		priv->screen_height = -1;
		priv->inited = 0;
		priv->state = 0;
		priv->num_old_bytes = 0;
		priv->local = NULL;
		gPanelList.list[gPanelList.num++] = priv;

		local->name = name;
		local->flags = 0 /* XI86_NO_OPEN_ON_INIT */;
		#ifndef XFREE86_V4
			local->device_config = xf86TKConfig;
		#endif
		local->device_control = xf86TKControl;
		local->read_input = xf86TKReadInput;
		local->control_proc = NULL;
		local->close_proc = NULL;
		local->switch_mode = NULL;
		local->conversion_proc = xf86TKConvert;
		local->reverse_conversion_proc = NULL;
		local->fd = -1;
		local->atom = 0;
		local->dev = NULL;
		local->private = priv;
		local->private_flags = flag;
		local->type_name = type_name;
		local->history_size = 0;

		return local;
	}else /* XI_CONTROL */
	{	
		return NULL;	
	}
}

/*
 ***************************************************************************
 *
 * xf86TKAllocatePanel --
 *
 ***************************************************************************
 */
static LocalDevicePtr
#ifndef XFREE86_V4
xf86TKAllocatePanel(void)
#else
xf86TKAllocatePanel(InputDriverPtr	drv)     
#endif
{
#ifndef XFREE86_V4
  LocalDevicePtr	local = xf86TKAllocate(XI_PANEL,  DRV_NAME " Panel", PANEL_ID);
#else
  LocalDevicePtr	local = xf86TKAllocate(drv, XI_PANEL,  DRV_NAME " Panel", PANEL_ID);
#endif

  if (local) {
    ((TKPanelPrivatePtr) local->private)->local = local;
  }
  return local;  
}




#ifndef XFREE86_V4 /* { */
/*
 ***************************************************************************
 *
 * TouchKit device association --
 *
 ***************************************************************************
 */
DeviceAssocRec TK_panel_assoc =
{
  PANEL_SECTION_NAME,		/* config_section_name */
  xf86TKAllocatePanel		/* device_allocate */
};


#ifdef DYNAMIC_MODULE /* { */
/*
 ***************************************************************************
 *
 * entry point of dynamic loading
 *
 ***************************************************************************
 */
int
#ifndef DLSYM_BUG
init_module(unsigned long	server_version)
#else
init_xf86TurboTouch(unsigned long      server_version)
#endif
{
    xf86AddDeviceAssoc(&TK_panel_assoc);

    if (server_version != XF86_VERSION_CURRENT) {
      ErrorF("Warning: " DRV_NAME " module compiled for version%s\n", XF86_VERSION);
      return 0;
    }
    else {
      return 1;
    }
}
#endif /* } */

#else /* XFREE86_V4 */ /* } else { */
static void
xf86TKUninit(InputDriverPtr	drv,
	      LocalDevicePtr	local,
	      int		flags)
{
  TKPanelPrivatePtr		priv = (TKPanelPrivatePtr) local->private;
  int i;
  
	for (i=0; i<gPanelList.num; i++)
	{	if (gPanelList.list[i] == priv)
		{	gPanelList.list[i] = gPanelList.list[--gPanelList.num];
			break;
		}
	}
  
  if (priv) {
    priv->local->private = NULL;
    xfree(priv->input_dev);
    xfree(priv);
  }
  xfree(local->name);
  xfree(local);

  xf86DeleteInput(local, 0);
}

static const char *default_options[] = {
  "BaudRate", "9600",
  "StopBits", "1",
  "DataBits", "8",
  "Parity", "None",
  "Vmin", "10",
  "Vtime", "1",
  "FlowControl", "None",
  NULL
};



static InputInfoPtr
xf86TKInit(InputDriverPtr	drv,
			IDevPtr			dev,
			int				flags)
{
	LocalDevicePtr local=NULL, fake_local=NULL;
	TKPanelPrivatePtr p_priv = NULL;
	char *str;

	fake_local = (LocalDevicePtr) xcalloc(1, sizeof(LocalDeviceRec));
	if (!fake_local)
	{	goto init_err;
	}
	fake_local->conf_idev = dev;
	
	xf86CollectInputOptions(fake_local, default_options, NULL);

	local = xf86TKAllocatePanel(drv);
	if (!local)
	{	goto init_err;
	}
	local->options = fake_local->options;
	local->conf_idev = fake_local->conf_idev;
	xfree(fake_local);
	fake_local = NULL;
	
	/* Process the common options. */
	xf86ProcessCommonOptions(local, local->options);
	xf86Msg(X_CONFIG,  DRV_NAME " X device name: %s\n", local->name);  

	str = xf86FindOptionValue(local->options, "device");
	if (!str)
	{	xf86Msg(X_ERROR, "%s: No Device specified in TouchKit module config.\n",
		dev->identifier);
		goto init_err;
	}
	{	p_priv = local->private;
		p_priv->input_dev = str;

		p_priv->screen_no = xf86SetIntOption(local->options, "ScreenNo", 0);
		xf86Msg(X_CONFIG,  DRV_NAME " associated screen: %d\n", p_priv->screen_no);  
		debug_level = xf86SetIntOption(local->options, "DebugLevel", 0);
		if (debug_level) 
		{
		#if DEBUG
			xf86Msg(X_CONFIG,  DRV_NAME " debug level sets to %d\n", debug_level);      
		#else
			xf86Msg(X_INFO,  DRV_NAME " debug not available\n");      
		#endif
		}
	}
  /* mark the device configured */
	local->flags |= XI86_CONFIGURED;
	return local;

init_err:
			
	if (fake_local)
	{	xfree(fake_local);
	}
	if (p_priv)
	{	if (p_priv->input_dev)
		{	xfree(p_priv->input_dev);
		}
		xfree(p_priv);
	}
	if (local)
	{	xfree(local);
	}
	return NULL;
}

#ifdef XFree86LOADER
	static
#endif
InputDriverRec EGTOUCH = {
    1,						/* driver version */
    DRV_NAME,			/* driver name */
    NULL,					/* identify */
    xf86TKInit,			/* pre-init */
    xf86TKUninit,			/* un-init */
    NULL,					/* module */
    0						/* ref count */
};

#ifdef XFree86LOADER /* { */
static pointer
Plug(pointer	module,
     pointer	options,
     int	*errmaj,
     int	*errmin)
{
  xf86AddInputDriver(&EGTOUCH, module, 0);

  return module;
}

static void
Unplug(pointer	p)
{
  DBG(1, ErrorF("TKUnplug\n"));
}

static XF86ModuleVersionInfo version_rec = {
  DRV_NAME,
  MODULEVENDORSTRING,
  MODINFOSTRING1,
  MODINFOSTRING2,
  XF86_VERSION_CURRENT,
  1, 0, 0,
  ABI_CLASS_XINPUT,
  ABI_XINPUT_VERSION,
  MOD_CLASS_XINPUT,
  { 0, 0, 0, 0 }
};

/*
 * This is the entry point in the module. The name
 * is setup after the pattern <module_name>ModuleData.
 * Do not change it.
 */

#define ModData(name)			IDcat(name, ModuleData)
 
XF86ModuleData ModData(XDRIVER_NAME) = { &version_rec, Plug, Unplug };
#endif /* } */

#endif /* XFREE86_V4 */ /* } */

