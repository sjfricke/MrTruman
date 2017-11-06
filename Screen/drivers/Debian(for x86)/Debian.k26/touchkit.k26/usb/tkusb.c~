
/* -*- linux-c -*- */
/*********************************************************************

	Driver for eGalax USB Touch Screen Controller

	Macbeth Chang, eGalax Inc., Aug. 2001.
	
 *********************************************************************/	 

// developed with kernel version 2.2.16, RedHat 7.0

#include "config.h"
#include "tkusb_dbg.h"
#include "tkusb.h"

static void OK_tscreen(struct urb *urb);

#define INT_Interval		5 //10

#if defined(VERBOSE) && defined(URB_STATUS_DBG) // {
static void PrintURB_Status(int status)
{
	#define Case(xx)	case xx: dbg("status = " #xx "\n"); break;
	switch (status)
	{	default: DBG("status = <undefined>\n");
		Case(USB_ST_CRC      )
		Case(USB_ST_BITSTUFF     )
		Case(USB_ST_NORESPONSE   )
		Case(USB_ST_DATAOVERRUN  )
		Case(USB_ST_DATAUNDERRUN )
		Case(USB_ST_BUFFEROVERRUN    )
		Case(USB_ST_BUFFERUNDERRUN   )
//		Case(USB_ST_INTERNALERROR    )
//		Case(USB_ST_SHORT_PACKET     )
		Case(USB_ST_PARTIAL_ERROR    )
		Case(USB_ST_URB_KILLED       )
		Case(USB_ST_URB_PENDING       )
		Case(USB_ST_REMOVED      )
//		Case(USB_ST_TIMEOUT      )
		Case(USB_ST_NOTSUPPORTED )
		Case(USB_ST_BANDWIDTH_ERROR  )
		Case(USB_ST_URB_INVALID_ERROR  )
		Case(USB_ST_URB_REQUEST_ERROR  )
		Case(USB_ST_STALL        )
	}
}
#else // } else {		
#define PrintURB_Status(s)
#endif // }

static ssize_t
DoSendData(struct scr_usb_data *scr, int len)
{	struct usb_device *dev = scr->scr_dev;

	scr->setup_pkt.BmRequestType	= 0x40; /* dir: H2D, type: Vendor, recv: Device */
	scr->setup_pkt.Brequest			= 0;
	scr->setup_pkt.Wvalue			= 0;
	scr->setup_pkt.Windex			= 0;
	scr->setup_pkt.Wlength			= len;

	DBG("send %d bytes\n", len);

#ifdef SEND_DATA_DBG
	{	int i;
		for (i=0; i<len; i++)
		{	printk("{%02X}", (unsigned char)scr->output[i]);
		}
		printk("\n");
	}
#endif
	
	FILL_CONTROL_URB(&scr->scr_out, dev, 
					usb_sndctrlpipe(dev, 0), (char*)&scr->setup_pkt,
					scr->output, len, OK_tscreen, scr);
					
	if (usb_submit_urb(&scr->scr_out)) 
	{	ERR("usb_submit_urb failed");
		return -EIO;
	}
	return 0;
}

static void
OK_tscreen(struct urb *urb)
{
	struct scr_usb_data *scr = urb->context;
//	struct usb_device *dev = scr->scr_dev;
//	__u8 *data = urb->transfer_buffer;
//  int len = urb->actual_length;

	DBG("write OK called with status %d\n", urb->status);

	if (urb->status)
	{	PrintURB_Status(urb->status);
		scr->obuf_wp = scr->obuf_rp = 0;
		scr->writing = 0;
		warn("write failure, erase outbuf buffer\n");
	}else
	{	int obsz = scr->obuf_wp - scr->obuf_rp;
		int dis = OBUF_SIZE - scr->obuf_rp;
		int alen;
		
		DBG("OK: WP %d, RP %d\n", scr->obuf_wp, scr->obuf_rp);

		if (obsz == 0)
		{	scr->writing = 0;
			return;
		}else if (obsz < 0)
		{	obsz += OBUF_SIZE;
		}
		
		alen = obsz < MAX_DATA_LEN ? obsz : MAX_DATA_LEN;
		alen = dis < alen ? dis : alen;
		
		memcpy(scr->output, scr->obuf+scr->obuf_rp, alen);
		scr->obuf_rp += alen;
		if (scr->obuf_rp == OBUF_SIZE)
		{	scr->obuf_rp = 0;
		}
		DoSendData(scr, alen);
		if (obsz < OBUF_SIZE/2 )
		{	wake_up_interruptible(&scr->wr_wait_q);
		}
	}
}



static void
irq_tscreen(struct urb *urb)
{	struct scr_usb_data *scr = urb->context;
	__u8 *data = urb->transfer_buffer;
    int len = urb->actual_length;

	DBG("irq called with status %d\n", urb->status);
	DBG("before irq: wp: %d, rp: %d\n", scr->ibuf_wp, scr->ibuf_rp);

	if (urb->status)
	{	PrintURB_Status(urb->status);
		
		#if 0
			if (scr->ibuf_wp > scr->ibuf_rp)
			{	int i;
				
				for (i=scr->ibuf_rp; i<scr->ibuf_wp; i++)
				{	printk("[%02X]", (unsigned char)scr->ibuf[i]);
				}
			}
			if (0)
			{	struct usb_device *dev = scr->scr_dev;
				FILL_INT_URB(&scr->scr_irq, dev, 
					     usb_rcvintpipe(dev, 1),
					     scr->input, 8, irq_tscreen, scr,
					     INT_Interval);
				if (usb_submit_urb(&scr->scr_irq)) 
				{	ERR("usb_submit_urb failed");
				}
			}
		#endif
		return;
	}
	WRITE_DATA_TO_QUEUE(scr, ibuf, IBUF_SIZE, data, len);
	
	DBG("after irq: H: %d, T: %d\n", scr->ibuf_wp, scr->ibuf_rp);

	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
		kill_fasync(scr->fasync, SIGIO, POLL_IN);
	#else
		kill_fasync(&scr->fasync, SIGIO, POLL_IN);
	#endif
	
	wake_up_interruptible(&scr->rd_wait_q);
	
	{	struct usb_device *dev = scr->scr_dev;

		FILL_INT_URB(&scr->scr_irq, dev, 
			     usb_rcvintpipe(dev, 1),
			     scr->input, 8, irq_tscreen, scr,
			     INT_Interval);
	}
	return;
}


static int
open_tscreen(struct inode * inode, struct file * file)
{
	struct scr_usb_data *scr;
	struct usb_device *dev;
	kdev_t scr_minor;
	int mode;

	scr_minor = USB_SCR_MINOR(inode);

	DBG("open_tscreen: scr_minor:%d\n", scr_minor);

	if (!p_scr_table[scr_minor])
	{	//ERR("open_tscreen(%d): Unable to access minor data\n", scr_minor);
		return -ENODEV;
	}
	scr = p_scr_table[scr_minor];
	
	down(scrSem + scr_minor);
	while (scr->closing)
	{	if ( !scr->present )
		{	up(scrSem + scr_minor);
			return -ENODEV;
		}
		if (signal_pending(current))
		{	up(scrSem + scr_minor);
			return -EINTR;
		}
		up(scrSem + scr_minor);
		interruptible_sleep_on(&scr->op_wait_q);
		down(scrSem + scr_minor);
	}
	up(scrSem + scr_minor);
	
	dev = scr->scr_dev;
	if (!dev) {
		ERR("open_tscreen(%d): TouchScreen device not present", scr_minor);
		return -ENODEV;
	}

	if (!scr->present) {
		ERR("open_tscreen(%d): TouchScreen is not present", scr_minor);
		return -ENODEV;
	}


	mode = file->f_flags & O_ACCMODE;
	if ( mode != O_WRONLY )
	{	if ( !scr->readcnt )
		{	DBG("here comes the first reader\n");
			scr->ibuf_wp = scr->ibuf_rp = 0;
			FILL_INT_URB(&scr->scr_irq, dev, 
					     usb_rcvintpipe(dev, 1),
					     scr->input, MAX_DATA_LEN,
					     irq_tscreen, scr, INT_Interval);
			if (usb_submit_urb(&scr->scr_irq)) 
			{	ERR("usb_submit_urb failed");
	    		return -EIO;
			}
		}
		scr->readcnt++;
	}
	if ( mode != O_RDONLY)
	{	scr->writecnt++;
	}
	scr->opencnt++;	

	DBG("open done:  rcnt: %d, wcnt %d, ocnt %d",
				scr->readcnt, scr->writecnt, scr->opencnt);

	file->private_data = scr; /* Used by the read and write metheds */

	MOD_INC_USE_COUNT;

	return 0;
}

static int 
fasync_tscreen(int fd, struct file *file, int on)
{	int retval;
	struct scr_usb_data *scr;

	scr = file->private_data;
	retval = fasync_helper(fd, file, on, &scr->fasync);
	return retval < 0 ? retval : 0;
}


static int
close_tscreen(struct inode * inode, struct file * file)
{
	struct scr_usb_data *scr;
	kdev_t scr_minor;
	int mode;

	SALUTE("close called\n");

	fasync_tscreen(-1, file, 0);

	scr_minor = USB_SCR_MINOR (inode);

	DBG("close_tscreen: scr_minor:%d\n", scr_minor);

	if (!p_scr_table[scr_minor]) {
		ERR("close_tscreen(%d): invalid scr_minor", scr_minor);
		return -ENODEV;
	}

	down(scrSem + scr_minor);
	scr = p_scr_table[scr_minor];
	scr->closing = 1;
	mode = file->f_flags & O_ACCMODE;
	
	if (mode != O_WRONLY)
	{	if ( !(--scr->readcnt) )
		{	if (scr->present)
			{	usb_unlink_urb(&scr->scr_irq);
			}
			scr->ibuf_wp = scr->ibuf_rp = 0;
		}
	}
	if (mode != O_RDONLY)

	{	if ( !(--scr->writecnt) )
		{	if (scr->present)
			{	do
		        {	ULONG flags;  save_flags(flags); cli();
					if (scr->writing)
					{	up(scrSem + scr_minor);
						interruptible_sleep_on(&scr->wr_wait_q);
						down(scrSem + scr_minor);
					}
					restore_flags(flags);
					if (signal_pending(current))
					{	break;
					}
				} while (scr->writing);
				usb_unlink_urb(&scr->scr_out);
			}
		}
	}
	if ( !(--scr->opencnt) )
	{	if (!scr->present)
		{	DBG("close_tscreen: De-allocating minor:%d\n", scr->scr_minor);
			kfree(scr->ibuf);
			kfree(scr->obuf);
			p_scr_table[scr->scr_minor] = NULL;
			kfree(scr);
			file->private_data = NULL;
			up(scrSem + scr_minor);
			MOD_DEC_USE_COUNT;
			return 0;
		}
	}
	DBG("close done:  rcnt: %d, wcnt %d, ocnt %d\n",
				scr->readcnt, scr->writecnt, scr->opencnt);

	scr->closing = 0;
	up(scrSem + scr_minor);
	wake_up_interruptible(&scr->op_wait_q);
	
	MOD_DEC_USE_COUNT;
	return 0;
}


static ssize_t
write_tscreen(struct file * file, const char * buffer,
              size_t count, loff_t *ppos)
{
	struct scr_usb_data *scr;
	int written = 0;
	int idx;

	SALUTE("write called\n");

	scr = file->private_data;
	
	if (!scr->present)
	{	return -ENODEV;
	}
	file->f_dentry->d_inode->i_atime = CURRENT_TIME;

	if ( verify_area(VERIFY_READ, (void*)buffer, count) )
	{	return -EFAULT;
	}

	{	int i;
		for (i=0; i<SCR_MAX_MNR; i++)
		{	if (p_scr_table[i] == scr)
			{	break;
			}	
		}
		if (i<SCR_MAX_MNR)
		{	idx = i;
		}else
		{	return -EINVAL;
		}
	}

	down(scrSem + idx);
	if (!scr->writing)
	{	int alen = count > MAX_DATA_LEN ? MAX_DATA_LEN : count;
		int rval;
		copy_from_user(scr->output, buffer, alen);
		scr->writing = 1;
		if ((rval = DoSendData(scr, alen)))
		{	warn("Can not init writing");
			return rval;
		}
		buffer += alen;
		count -= alen;
		written += alen;
	}

	DBG("(%d) write: wp: %d, rp: %d\n", __LINE__, scr->obuf_wp, scr->obuf_rp);

	{	int obsz = scr->obuf_wp - scr->obuf_rp;
	
		if (obsz < 0)
		{	obsz += OBUF_SIZE;
		}
		if (obsz + count > OBUF_SIZE - 1)
		{	up(scrSem + idx);
			interruptible_sleep_on(&scr->wr_wait_q);
			down(scrSem + idx);
		}else
		{	int dis = OBUF_SIZE - scr->obuf_wp;
			int alen = dis < count ? dis : count;
			copy_from_user(scr->obuf+scr->obuf_wp, buffer, alen);
			scr->obuf_wp += alen;
			if (scr->obuf_wp == OBUF_SIZE)
			{	scr->obuf_wp = 0;
			}
			if (alen < count)
			{	copy_from_user(scr->obuf+scr->obuf_wp, buffer+alen, count-alen);
				scr->obuf_wp += count-alen;
			}
			written += count;
		}
	}
	up(scrSem + idx);
	
	return written;
}

static ssize_t
read_tscreen(struct file * file, char *dest,
             size_t size, loff_t *ppos)
{	
	struct scr_usb_data *scr;
	int is_nblk = file->f_flags & O_NONBLOCK;
	int totalrd = 0;
	int actualrd, datalen;
	int idx;

	SALUTE("read called\n");

	scr = file->private_data;
	
	if (!scr->present)
	{	return -ENODEV;
	}
	file->f_dentry->d_inode->i_atime = CURRENT_TIME; /* Update the atime of inode */

	if ( verify_area(VERIFY_WRITE, dest, size) )
	{	return -EFAULT;
	}

	{	int i;
		for (i=0; i<SCR_MAX_MNR; i++)
		{	if (p_scr_table[i] == scr)
			{	break;
			}	
		}
		if (i<SCR_MAX_MNR)
		{	idx = i;
		}else
		{	return -EINVAL;
		}
	}

	do 
	{	if ( scr->ibuf_rp == scr->ibuf_wp )
		{	if (is_nblk) 
			{   return -EAGAIN;
			}
	        do
	        {	ULONG flags;  save_flags(flags); cli();
				if (scr->ibuf_rp == scr->ibuf_wp)
				{	interruptible_sleep_on(&scr->rd_wait_q);
				}
				restore_flags(flags);
				if (signal_pending(current))
				{	return -ERESTARTSYS;
				}
			} while (scr->ibuf_rp == scr->ibuf_wp);
		}
	
		down(scrSem + idx);
		if ( scr->ibuf_rp > scr->ibuf_wp )
		{	datalen = IBUF_SIZE - scr->ibuf_rp;
			actualrd = datalen <= size ? datalen : size;
			copy_to_user(dest, scr->ibuf + scr->ibuf_rp, actualrd);
			size -= actualrd;
			dest += actualrd;
			totalrd += actualrd;
			scr->ibuf_rp = 0;
		}
		if ( scr->ibuf_rp < scr->ibuf_wp )
		{	datalen = scr->ibuf_wp - scr->ibuf_rp;
			actualrd = datalen <= size ? datalen : size;
			copy_to_user(dest, scr->ibuf + scr->ibuf_rp, actualrd);
			scr->ibuf_rp += actualrd;
			totalrd += actualrd;
		}
		up(scrSem + idx);
	} while (!totalrd);

	return totalrd;	
}

static unsigned int
poll_tscreen(struct file *file, struct poll_table_struct *wait)
{	struct scr_usb_data *scr;
	int rval = 0;

	SALUTE("poll called\n");

	scr = file->private_data;
	
	if (!scr->present)
	{	rval = POLLERR | POLLHUP;
	} else
	{	if (file->f_mode & FMODE_WRITE) 
		{	int obsz = scr->obuf_wp - scr->obuf_rp;
		
			if (obsz < 0)
			{	obsz += OBUF_SIZE;
			}
			if (obsz < OBUF_SIZE - MAX_DATA_LEN)
			{	rval |= POLLOUT | POLLWRNORM;
			} else 
			{	// poll_wait(file, &scr->wd_wait_q, wait); ??
			}
		}
		if (file->f_mode & FMODE_READ) 
		{	if (scr->ibuf_wp == scr->ibuf_rp)
			{	
		//		interruptible_sleep_on(&scr->rd_wait_q);
			
				poll_wait(file, &scr->rd_wait_q, wait);
			}else
			{	rval |= POLLIN | POLLRDNORM;
			}
		}
	}
	return rval;
}

#ifdef IOCTL_SHOW_STATUS

static int ioctl_tscreen(struct inode *inode, struct file * file,
              unsigned int cmd, unsigned long arg)
{	struct scr_usb_data *scr;
	int rval = -EINVAL;

	SALUTE("ioctl called");

	if ( !(scr=file->private_data) )
	{	return rval;
	}
	switch (cmd)
	{
	case SCR_IOCTL_SHOW_STATUS:
	{	ModuleStatus stat;
		rval = verify_area(VERIFY_WRITE, (void*)arg, sizeof(ModuleStatus));
		if ( !rval )
		{	copy_to_user((ModuleStatus*)arg, &stat, sizeof(ModuleStatus));
		}
		break;
	}
	}
	return rval;
}
#endif

#ifdef MODULE

static int str2int(char *src, long *dest)
{	int i;
	long rval;

	for ( rval=i=0; i<4 && src[i]; i++, rval <<= 4 )
	{	if ( '0' <= src[i] && src[i] <= '9' )
		{	rval += src[i] - '0';
		}else if ( 'a' <= src[i] && src[i] <= 'f' )
		{	rval += src[i] - 'a' + 10;
		}else if ( 'A' <= src[i] && src[i] <= 'F' )
		{	rval += src[i] - 'A' + 10;
		}
	}
	if ( i < 4 )
	{	return 0;
	}else
	{	*dest = rval >> 4;
		return 1;
	}
}

static void RemoveSpace(char *victim)
{	int r, w;
	
	for ( r=w=0; victim[r]; r++)
	{	if ( victim[r] != ' ' )
		{	victim[w++] = victim[r];
		}
	}
	victim[w] = '\0';
}

static int
MatchIDlist(long vid, long pid)
{	int i;
	long lvid, lpid;
	static int compact = 0;

	DBG("vidlist is %s\n", vidlist ? vidlist : "(nil)");
	DBG("pidlist is %s\n", pidlist ? pidlist : "(nil)");
	
	if ( !vidlist || !pidlist )
		return 0;

	if ( !compact )
	{	RemoveSpace(vidlist);
		RemoveSpace(pidlist);

	DBG("vidlist is %s\n", vidlist ? vidlist : "(nil)");
	DBG("pidlist is %s\n", pidlist ? pidlist : "(nil)");
	
		compact = 1;
	}
	
	for (i=0; ; i+=4)
	{	if ( str2int(vidlist+i, &lvid) && str2int(pidlist+i, &lpid) )
		{	DBG("Try vid %04lX, pid %04lX...", lvid, lpid);
			if ( lvid == vid && lpid == pid )
			{	DBG("OK\n");
				return 1;
			}else
			{	DBG("failed\n");
			}
		}else
		{	break;
		}
	}
	return 0;
}
#endif


static void *
probe_tscreen(struct usb_device *dev, unsigned int ifnum
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
		,const struct usb_device_id *id	
	#endif
			)
{
	struct scr_usb_data *scr;
	struct usb_interface_descriptor *interface;
	struct usb_endpoint_descriptor *endpoint;
	kdev_t scr_minor;

	SALUTE("probe called\n");

	if ( vendor != IDcat(PRJNAME, _USB_VID) 
			|| product != IDcat(PRJNAME, _USB_PID) )
	{	INFO("probe_tscreen: User specified USB tscreen -- Vendor:Product - %x:%x", vendor, product);
	}
	DBG("probe_tscreen: USB dev address:%p\n", dev);
	DBG("probe_tscreen: ifnum:%u\n", ifnum);

	DBG("vendor %d, product %d\n", vendor, product);

	if (dev->descriptor.idVendor != vendor ||
	    dev->descriptor.idProduct != product)  
	{	
		#ifdef MODULE
			if ( !MatchIDlist(
					dev->descriptor.idVendor, dev->descriptor.idProduct) )
		#endif
		{	return NULL;    /* We didn't find anything pleasing */
		}
	}

/*
 * After this point we can be a little noisy about what we are trying to
 *  configure.
 */
/* 
 	if ( dev->descriptor.bDeviceClass != USB_CLASS_VENDOR_SPEC )
 	{	info("probe_tscreen: Only device of vendor class is supported."); 
 		return NULL;
 	}
*/
	if (dev->descriptor.bNumConfigurations != 1) {
		info("probe_tscreen: Only one device configuration is supported.");
		return NULL;
	}
	if (dev->config[0].bNumInterfaces != 1) {
		info("probe_tscreen: Only one device interface is supported.");
		return NULL;
	}
	interface = dev->config[0].interface[ifnum].altsetting;
	endpoint = interface[ifnum].endpoint;
	if (interface->bNumEndpoints != 1) {
		info("probe_tscreen: Only one endpoint is supported.");
		return NULL;
	}
	if (! IS_EP_INTR(endpoint[0]))
	{	info("probe_tscreen: Endpoint of unexpected type. Notify the maintainer.");
		return NULL;	/* Shouldn't ever get here unless we have something weird */
	}

/* 
 * Determine a minor number and initialize the structure associated
 * with it.  The problem with this is that we are counting on the fact
 * that the user will sequentially add device nodes for the tscreen
 * devices.  */

	for (scr_minor = 0; scr_minor < SCR_MAX_MNR; scr_minor++)
	{	if (!p_scr_table[scr_minor] || !p_scr_table[scr_minor]->present)
			break;
	}

/* Check to make sure that the last slot isn't already taken */
	if (! (scr_minor < SCR_MAX_MNR) )
	{	ERR("probe_tscreen: No more minor devices remaining.");
		return NULL;
	}

	if ( !p_scr_table[scr_minor] )
	{	
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
		scrSem[scr_minor] = MUTEX; 
	#else
		init_MUTEX(scrSem+scr_minor);
	#endif
	
		DBG("probe_tscreen: (new panel) Allocated minor:%d\n", scr_minor);
	
		if (!(scr = kmalloc (sizeof (struct scr_usb_data), GFP_KERNEL))) {
			ERR("probe_tscreen: Out of memory.");
			return NULL;
		}
	
		memset (scr, 0, sizeof(struct scr_usb_data));
		dbg ("probe_tscreen(%d): Address of scr:%p\n", scr_minor, scr);
	
	/* Ok, now initialize all the relevant values */
		if (!(scr->obuf = (char *)kmalloc(OBUF_SIZE, GFP_KERNEL))) {
			ERR("probe_tscreen(%d): Not enough memory for the output buffer.", scr_minor);
			kfree(scr);
			return NULL;
		}
		DBG("probe_tscreen(%d): obuf address:%p\n", scr_minor, scr->obuf);
		scr->obuf_rp = scr->obuf_wp = 0;
	
		if (!(scr->ibuf = (char *)kmalloc(IBUF_SIZE, GFP_KERNEL))) {
			ERR("probe_tscreen(%d): Not enough memory for the input buffer.", scr_minor);
			kfree(scr->obuf);
			kfree(scr);
			return NULL;
		}
		DBG("probe_tscreen(%d): ibuf address:%p\n", scr_minor, scr->ibuf);
		scr->ibuf_wp = scr->ibuf_rp = 0;
	
		scr->present = 1;
		scr->scr_dev = dev;
		scr->scr_minor = scr_minor;
		scr->closing = 0;
		scr->opencnt = 0;
		scr->writecnt = 0;
		scr->readcnt = 0;
		scr->fasync = NULL;
		init_waitqueue_head(&scr->rd_wait_q);
		init_waitqueue_head(&scr->wr_wait_q);
		init_waitqueue_head(&scr->op_wait_q);
	
		return p_scr_table[scr_minor] = scr;
	}else if (! p_scr_table[scr_minor]->present)
	{	// reconnect
		DBG("probe_tscreen: (reconnect) Allocated minor:%d", scr_minor);
		scr = p_scr_table[scr_minor];

	/*	DBG("probe_tscreen(%d): obuf address:%p\n", scr_minor, scr->obuf);
		scr->obuf_rp = scr->obuf_wp = 0;
		DBG("probe_tscreen(%d): ibuf address:%p\n", scr_minor, scr->ibuf);
		scr->ibuf_wp = scr->ibuf_rp = 0;
	*/
		if (scr->readcnt)
		{	DBG("probe_tscreen(%d): there %s %d reader(s) active",
						scr_minor, scr->readcnt == 1 ? "is" : "are", scr->readcnt);	
			FILL_INT_URB(&scr->scr_irq, dev, 
					     usb_rcvintpipe(dev, 1),
					     scr->input, MAX_DATA_LEN,
					     irq_tscreen, scr, INT_Interval);
			if (usb_submit_urb(&scr->scr_irq)) 
			{	ERR("usb_submit_urb failed");
			}
		}
		scr->present = 1;
		scr->scr_dev = dev;
		scr->scr_minor = scr_minor;
		
		return scr;
	}
	return NULL;
}

static void
disconnect_tscreen(struct usb_device *dev, void *ptr)
{
	struct scr_usb_data *scr = (struct scr_usb_data *) ptr;
	
	SALUTE("disconnect called %p\n", ptr);
		
	scr->present = 0;
	usb_unlink_urb(&scr->scr_irq);
	if (scr->readcnt)
	{	DBG("disconnect_tscreen(%d): Unlinking IRQ URB", scr->scr_minor);
		wake_up_interruptible(&scr->rd_wait_q);
	}
	if (scr->writing)
	{	DBG("disconnect_tscreen(%d): Unlinking OUT URB", scr->scr_minor);
		usb_unlink_urb(&scr->scr_out);
		scr->writing = 0;
	}
	if (scr->closing)
	{	wake_up_interruptible(&scr->wr_wait_q);
	}
	usb_driver_release_interface(&tscreen_driver,
                &scr->scr_dev->actconfig->interface[0]);

	DBG("after disconnect:  rcnt: %d, wcnt %d, ocnt %d\n",
				scr->readcnt, scr->writecnt, scr->opencnt);

	if (!scr->opencnt)
	{	DBG("disconnect_tscreen: De-allocating minor:%d\n", scr->scr_minor);
		kfree(scr->ibuf);
		kfree(scr->obuf);
		p_scr_table[scr->scr_minor] = NULL;
		kfree(scr);
	}
}


static struct
file_operations usb_tscreen_fops = {
	read:		read_tscreen,
	write:		write_tscreen,
	poll:		poll_tscreen,
	open:		open_tscreen,
	release:	close_tscreen,
	fasync:		fasync_tscreen,
	#ifdef IOCTL_SHOW_STATUS
	ioctl:		ioctl_tscreen
	#endif
};

static struct
usb_driver tscreen_driver = {
       .name = IDcat(PRJNAME, _USBMODULE),
       .probe = probe_tscreen,
       .disconnect = disconnect_tscreen,
/*       { NULL, NULL },
*/
       .fops = &usb_tscreen_fops,
       .minor = SCR_BASE_MNR
};

void __exit
usb_tscreen_exit(void)
{	usb_deregister(&tscreen_driver);
}

int __init
usb_tscreen_init (void)
{
    if (usb_register(&tscreen_driver) < 0)
    	return -1;

	info("USB TouchScreen support registered.\n");
	return 0;
}

module_init(usb_tscreen_init);
module_exit(usb_tscreen_exit);
