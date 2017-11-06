
#ifndef _XF86TOUCHKIT_H_ /* { */
#define _XF86TOUCHKIT_H_


#define PANEL_ID			1
#define CNTRL_ID			2
#define DEVICE_ID(flags)	((flags) & 0x03)


/************************************************************************
 *	
 *	Driver/Module Options
 *	
 ************************************************************************/

#define PORT			1
#define DEVICENAME		2
#define SCREEN_NO		3
#define MAPPING			4
#define DEBUG_LEVEL		5
#define LINK_SPEED		6
#define MOUSE_MODE		7
#define SELECT_DELAY	8
#define CLICK_DELAY		9
#define INPLACE			10
#define RCLICK_TOOL		11
#define ALWAYS_CORE		12

#define TK_BUFFER_SIZE		256	/* Size of input buffer.			*/
#define TK_PACKET_SIZE		10	/* Maximum size of a command/reply *including*	*/


/************************************************************************
 *	
 *	class eTurboTouchPanel{}
 *	
 ************************************************************************/

typedef unsigned char uchar;

typedef int TouchMode;

typedef struct
{	char		*input_dev;		/* The touchscreen input tty				*/
	int			link_speed;		/* Speed of the RS232 link					*/
	int			screen_no;		/* Screen associated with the device		*/
	int			screen_width;	/* Width of the associated X screen			*/
	int			screen_height;	/* Height of the screen						*/
	Bool		inited;			/* Has the controller been configured?		*/
	char		state;			/* Current state of report flags.			*/
	LocalDevicePtr	local;		/* Panel device ptr associated with the hw.*/
	int			num_old_bytes;	/* Number of bytes left in receive buffer.	*/
	uchar		rec_buf[TK_BUFFER_SIZE]; /* Receive buffer.						*/
	TouchMode	mode;
	int			touched;
}	TKPanelPrivateRec, *TKPanelPrivatePtr;


#endif /* } */

