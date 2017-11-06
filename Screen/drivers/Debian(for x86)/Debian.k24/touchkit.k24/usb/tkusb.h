
/*********************************************************************

	Driver for eGalax USB Touch Screen Controller

	Macbeth Chang, eGalax Inc., Aug. 2001.
	
 *********************************************************************/	 

#ifndef _EGTOUCH_H_
#define _EGTOUCH_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/ioctl.h>
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/sched.h>

#include <asm/semaphore.h>

#include <linux/usb.h>

#include "config.h"
#include "tpanel.h"

static __s32 vendor		= IDcat(PRJNAME, _USB_VID);
static __s32 product	= IDcat(PRJNAME, _USB_PID);

#ifdef MODULE
	static char *vidlist  = NULL;
	static char *pidlist  = NULL;

	MODULE_AUTHOR("eGalax Inc. http://www.egalax.com.tw");
	MODULE_DESCRIPTION("USB TouchScreen Driver");
	MODULE_PARM(vidlist, "s");
	MODULE_PARM(pidlist, "s");
#endif

#define IS_EP_BULK(ep)  ((ep).bmAttributes == USB_ENDPOINT_XFER_BULK ? 1 : 0)
#define IS_EP_BULK_IN(ep) (IS_EP_BULK(ep) && ((ep).bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN)
#define IS_EP_BULK_OUT(ep) (IS_EP_BULK(ep) && ((ep).bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_OUT)
#define IS_EP_INTR(ep) ((ep).bmAttributes == USB_ENDPOINT_XFER_INT ? 1 : 0)

#define USB_SCR_MINOR(X) MINOR((X)->i_rdev) - SCR_BASE_MNR

#ifdef DEBUG
#define SCR_DEBUG(X) X
#else
#define SCR_DEBUG(X)
#endif

#define IBUF_SIZE 4000
#define OBUF_SIZE 1000


struct scr_usb_data {
	struct usb_device *scr_dev;
	struct urb scr_irq;
	struct urb scr_out;
	kdev_t scr_minor;	/* TouchScreen minor - used in disconnect() */
	char opencnt;
	char readcnt;		/* Not zero if scr_irq urb is submitted */
	char writecnt;
	char writing;		/* Not zero if scr_out urb is not complete yet */
	char present;		/* Not zero if device is present */
	char closing;
	char input[8];	
	char output[8];
	struct {
		__u8	BmRequestType;
		__u8	Brequest;
		__u16	Wvalue;
		__u16	Windex;
		__u16	Wlength;
	}	setup_pkt;	
	char *obuf, *ibuf;	/* transfer buffers */
	int ibuf_wp, ibuf_rp, obuf_wp, obuf_rp;
	wait_queue_head_t rd_wait_q, wr_wait_q, op_wait_q; /* blocking I/O */
	struct fasync_struct *fasync;
};

static struct scr_usb_data *p_scr_table[SCR_MAX_MNR] = { NULL, /* ... */};
static struct semaphore scrSem[SCR_MAX_MNR];

#define MAX_DATA_LEN		8

typedef unsigned char	UCHAR;
typedef unsigned short	USHORT;
typedef unsigned long	ULONG;
 
#define WRITE_DATA_TO_QUEUE(port, bufname, MAX, src, len)			\
{	UCHAR *_BUF = (port)->bufname;									\
	USHORT _RP = (port)->bufname##_rp;								\
	USHORT _WP = (port)->bufname##_wp;								\
	USHORT _space = _RP > _WP ?										\
						(_RP-_WP - 1) :								\
						((MAX) -1 -(_WP-_RP));						\
	USHORT _len = (len) < _space ? (len) : _space;					\
																	\
	if ( _WP + _len <= (MAX) )										\
	{	memcpy(	_BUF + _WP, (src), _len);							\
		_WP += _len;												\
		_WP %= (MAX);												\
		(port)->bufname##_wp = _WP;									\
	}else															\
	{	USHORT _headlen = (MAX) - _WP;								\
		_len -= _headlen;											\
		memcpy( _BUF + _WP, (src), _headlen);						\
		memcpy( _BUF, (src) + _headlen, _len);						\
		(port)->bufname##_wp = _len;								\
	}																\
}

/* Forward declarations */
static struct usb_driver tscreen_driver;

#endif

