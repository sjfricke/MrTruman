

/* -*- linux-c -*- */
/*********************************************************************

	Driver for eGalax USB Touch Screen Controller

	Macbeth Chang, eGalax Inc., Aug. 2001.
	
 *********************************************************************/	 

// developed with kernel version 2.2.16, RedHat 7.0

#include "tkusb_dbg.h"
#include "tkusb.h"

#define FILL_INT_URB        usb_fill_int_urb
#define FILL_CONTROL_URB    usb_fill_control_urb
#define INT_Interval		5 //10
static struct semaphore     disconnect_sem;
static void OK_tscreen(struct urb *urb, struct pt_regs *regs );
static ssize_t DoSendData(struct scr_usb_data *scr, int len)
{	
    struct usb_device *dev = scr->scr_dev;
	scr->setup_pkt.BmRequestType	= 0x40; /* dir: H2D, type: Vendor, recv: Device */
	scr->setup_pkt.Brequest			= 0;
	scr->setup_pkt.Wvalue			= 0;
	scr->setup_pkt.Windex			= 0;
	scr->setup_pkt.Wlength			= cpu_to_le16( len );

	KdPrint( DBG_WRITE, ("send %d bytes\n", len) );
#ifdef SEND_DATA_DBG
	{	int i;
		for (i=0; i<len; i++)
		{	
            KdPrint( DBG_WRITE, ("{%02X}", (unsigned char)scr->output[i]) );
		}
		KdPrint( DBG_WRITE, ("\n") );
	}
#endif
    FILL_CONTROL_URB( scr->wrUrb, dev, 
					 usb_sndctrlpipe(dev, 0),
                     (char*)&scr->setup_pkt,
					 scr->output, 
                     len, 
                     OK_tscreen, 
                     scr );
	if ( usb_submit_urb( scr->wrUrb, GFP_KERNEL )) 
	{	
        //scr->writing = 0;
        //atomic_set( &scr->wr_busy, 0 );
        KdPrint( DBG_WRITE, ("usb_submit_urb failed") );
		return -EIO;
	}
	return 0;
}

static void OK_tscreen(struct urb *urb, struct pt_regs *regs )
{
	struct scr_usb_data *scr = urb->context;
	KdPrint( DBG_WRITE, ("write OK called with status %d\n", urb->status) );
    if (urb->status)
	{ 
		scr->obuf_wp = scr->obuf_rp = 0;
        atomic_set( &scr->wr_busy, 0 );
        complete( &scr->wr_done );
		KdPrint(DBG_WRITE, ("write failure, erase outbuf buffer\n"));
	}
    else
	{	
        int obsz = scr->obuf_wp - scr->obuf_rp;
		int dis = OBUF_SIZE - scr->obuf_rp;
		int alen;
		KdPrint( DBG_WRITE, ( "OK: WP %d, RP %d\n", scr->obuf_wp, scr->obuf_rp) );
		if (obsz == 0)
		{	
            atomic_set( &scr->wr_busy, 0 );
			return;
		}else if (obsz < 0)
		{	
            obsz += OBUF_SIZE;
		}
		
		alen = obsz < MAX_DATA_LEN ? obsz : MAX_DATA_LEN;
		alen = dis < alen ? dis : alen;
		
		memcpy(scr->output, scr->obuf+scr->obuf_rp, alen);
		scr->obuf_rp += alen;
		if (scr->obuf_rp == OBUF_SIZE)
		{	scr->obuf_rp = 0;
		}
        if (obsz < OBUF_SIZE/2 )
		{	
            wake_up_interruptible(&scr->wr_wait_q);
		}
		if( DoSendData(scr, alen) )
        {
            atomic_set( &scr->wr_busy, 0 );
            complete( &scr->wr_done );
        }
	}
}

static void  irq_tscreen(struct urb *urb, struct pt_regs *regs)
{	
    struct scr_usb_data *scr = urb->context;
	__u8 *data = urb->transfer_buffer;
    int len = urb->actual_length;
    struct usb_device *dev;
    
    dev = ( struct usb_device *)scr->scr_dev;
	KdPrint( DBG_READ, ( "irq called with status %d\n", urb->status) );
	KdPrint( DBG_READ, ( "before irq: wp: %d, rp: %d\n", scr->ibuf_wp, scr->ibuf_rp) );

	if (urb->status)
	{	
        //PrintURB_Status(urb->status);
        atomic_set( &scr->rd_busy, 0 );
        complete( &scr->rd_done );
		return;
	}
    if( scr->readcnt ) // If someone reading now! put the read data into the buffer!
    {
        KdPrint( DBG_READ, ("Someone reading now\n") )
        KdPrint( DBG_READ, ("after irq: H: %d, T: %d\n", scr->ibuf_wp, scr->ibuf_rp) );
    	WRITE_DATA_TO_QUEUE(scr, ibuf, IBUF_SIZE, data, len);
        KdPrint( DBG_READ, (" Wake up interruptible in IRQ\n") )
    	wake_up_interruptible(&scr->rd_wait_q);
    }
    else
    {
        KdPrint( DBG_READ, ("Nobody read now! Just throw away\n") );
    }
   if( scr->fasync )
    {
            KdPrint( DBG_READ, (" kill_fasync \n") );
            kill_fasync(&scr->fasync, SIGIO, POLL_IN);
    }
   FILL_INT_URB(scr->rdUrb,
                     dev, 
			         usb_rcvintpipe(dev, 1),
			         scr->input, 
                     8, 
                     irq_tscreen, 
                     scr,
			         INT_Interval );
   if (usb_submit_urb( scr->rdUrb, GFP_KERNEL )) 
	{	
            atomic_set( &scr->rd_busy, 0 );
            // completion( &scr->rd_done );
            KdPrint( DBG_READ, ("usb_submit_urb again for next poll ") );
	}
    return;
}

static void delete_tscreen( struct scr_usb_data *scr )
{
     KdPrint( (DBG_USB|DBG_CLOSE), ("Enter delete_tscreen \n"));
     usb_free_urb( scr->rdUrb );
     usb_free_urb( scr->wrUrb );
     kfree(scr->ibuf);
	 kfree(scr->obuf);
     up( &scr->scrSem );
	 kfree(scr);
     KdPrint( (DBG_USB|DBG_CLOSE), (" leave delete_tscreen \n"));
}

static int lauch_int_read( struct scr_usb_data *scr )
{
        struct usb_device *dev;
        dev = scr->scr_dev;
        KdPrint( DBG_OPEN,("here comes the first reader! Launch the Read Urb Now!\n"));
		scr->ibuf_wp = scr->ibuf_rp = 0;
		FILL_INT_URB( scr->rdUrb, dev, 
					  usb_rcvintpipe(dev, 1),
					  scr->input,
                      MAX_DATA_LEN,
					  irq_tscreen, 
                      scr, 
                      INT_Interval);
        atomic_set( &scr->rd_busy, 1 );
		if (usb_submit_urb(scr->rdUrb, GFP_KERNEL )) 
		{	
                KdPrint( DBG_OPEN, ("usb_submit_urb failed") );
                atomic_set( &scr->rd_busy, 0 );
                //up( &disconnect_sem );
	    		return -EIO;
		}
        return 0;
}
static int open_tscreen(struct inode * inode, struct file * file)
{
	struct scr_usb_data *scr;
    struct usb_device *dev;
    struct usb_interface *pIf;
	int scr_minor;
	int mode;
    KdPrint( DBG_OPEN, (" Enter open_tscreen \n") );
    scr_minor = iminor( inode );
	KdPrint( DBG_OPEN, ("open_tscreen: scr_minor:%d\n", scr_minor) );
    down( &disconnect_sem );                            // acquire the mutex
    KdPrint( DBG_OPEN, (" find interface according to scr_minor\n") );
    pIf = usb_find_interface( &tscreen_driver , scr_minor );
    if( !pIf )
    {
        KdPrint( DBG_OPEN, (" cannot find such device at scr_minor=%d\n",scr_minor ) );
        up( &disconnect_sem );
        return -ENODEV;
    }
    scr = usb_get_intfdata( pIf ); 
    if( NULL == scr )
    {
        up( &disconnect_sem );
        return -ENODEV;
    }
    if( down_interruptible( &scr->scrSem ) )
    {
        up( &disconnect_sem );
        return -ERESTARTSYS;
    }
	while( scr->closing )
	{   
		up(&scr->scrSem);
		//interruptible_sleep_on(&scr->op_wait_q);
        KdPrint( DBG_OPEN, (" Closing now! Wait until close done\n") );
        if( wait_event_interruptible( scr->op_wait_q, (scr->closing==0) ) )
        {
            KdPrint( DBG_OPEN, (" Clsoing wait_event_interruptible failure\n"));
            up( &disconnect_sem );
            return -ERESTARTSYS;
        } 
		if( down_interruptible(&scr->scrSem) )
        {
          up( &disconnect_sem );
          return -ERESTARTSYS;  
        }
	}
	dev = scr->scr_dev;
	if (!dev) 
    {
		KdPrint( DBG_OPEN, ("open_tscreen(%d): TouchScreen device not present", scr_minor) );
        up(&scr->scrSem);
        up( &disconnect_sem );
        return -ENODEV;
	}
	if (!scr->present) 
   {
		KdPrint( DBG_OPEN, ("open_tscreen(%d): TouchScreen is not present", scr_minor) );
        up(&scr->scrSem);
        up( &disconnect_sem );
        return -ENODEV;
	}
    /*=======================================*/
    /* Read Urb not launched! Launched now   */
    /*=======================================*/
    if( !atomic_read( &scr->rd_busy ) )
    {
        if( lauch_int_read( scr ) )
        {
          up(&scr->scrSem);
          up( &disconnect_sem );
          return -EIO;
        }
    }
	mode = file->f_flags & O_ACCMODE;
	if ( mode != O_WRONLY )
	{	
		scr->readcnt++;
	}
	if ( mode != O_RDONLY)
	{	
        scr->writecnt++;
	}
	scr->opencnt++;	
	KdPrint( DBG_OPEN, ("open done:  rcnt: %d, wcnt %d, ocnt %d",
			scr->readcnt, scr->writecnt, scr->opencnt) );
	file->private_data = scr; /* Usedjn by the read and write metheds */
	//MOD_INC_USE_COUNT;
	up(&scr->scrSem);
    up( &disconnect_sem );
	return 0;
}

static int fasync_tscreen(int fd, struct file *file, int on)
{	int retval;
	struct scr_usb_data *scr;
    KdPrint( DBG_ASYNC, (" Enter fasync_tscreen\n") );
	scr = file->private_data;
	retval = fasync_helper(fd, file, on, &scr->fasync);
    KdPrint( DBG_ASYNC, (" Leave fasync_tscreen - ret =%d\n",retval ) );
	return retval < 0 ? retval : 0;
}


static int close_tscreen(struct inode * inode, struct file * file)
{
	struct scr_usb_data *scr;
    unsigned char scr_minor;
	int mode;
    KdPrint( DBG_CLOSE, (" Enter close_tscreen \n") );
    scr = ( struct scr_usb_data *) file->private_data;
    if( !scr )
    {
        KdPrint( DBG_CLOSE, (" close called! But no data\n") );
        return -1;
    }
	fasync_tscreen(-1, file, 0);
	scr_minor = scr->scr_minor;
	KdPrint( DBG_CLOSE, ("close_tscreen: scr_minor:%d\n", scr_minor) );
    if( down_interruptible( &scr->scrSem ) )
    {
        return -ERESTARTSYS;
    }
   	scr->closing = 1;
	mode = file->f_flags & O_ACCMODE;
	if (mode != O_WRONLY)
	{  
        scr->readcnt--;
	}
	if (mode != O_RDONLY)
	{  
        scr->writecnt--;
    }
	if ( !(--scr->opencnt) )
	{	
        if (!scr->present)
		{	
            KdPrint( DBG_CLOSE, ("close_tscreen: De-allocating minor:%d\n", scr->scr_minor) );
		    //
            // Unlink until read,write operation done!
            delete_tscreen( scr );
			file->private_data = NULL;
			//MOD_DEC_USE_COUNT;
			return 0;
		}
	}
	KdPrint( DBG_CLOSE, ("close done:  rcnt: %d, wcnt %d, ocnt %d\n",
			scr->readcnt, scr->writecnt, scr->opencnt));
	scr->closing = 0;
	up(&scr->scrSem);
	wake_up_interruptible(&scr->op_wait_q);
	//MOD_DEC_USE_COUNT;
	return 0;
}

/**
 * verify_area: - Obsolete/deprecated and will go away soon,
 * use access_ok() instead.
 * @type: Type of access: %VERIFY_READ or %VERIFY_WRITE
 * @addr: User space pointer to start of block to check
 * @size: Size of block to check
 *
 * Context: User context only.  This function may sleep.
 *
 * This function has been replaced by access_ok().
 *
 * Checks if a pointer to a block of memory in user space is valid.
 *
 * Returns zero if the memory block may be valid, -EFAULT
 * if it is definitely invalid.
 *
 * See access_ok() for more details.
 */
static inline int __deprecated VERIFY_AREA(int type, const void __user * addr, unsigned long size)
{
	return access_ok(type,addr,size) ? 0 : -EFAULT;
}

static ssize_t write_tscreen(struct file * file, const char * buffer,
                             size_t count, loff_t *ppos)
{
	struct scr_usb_data *scr;
	int written = 0;
	KdPrint( DBG_WRITE, ("write called\n") );
	scr = file->private_data;
	if (!scr->present)
	{	
        return -ENODEV;
	}
	file->f_dentry->d_inode->i_atime = CURRENT_TIME;
	if ( VERIFY_AREA(VERIFY_READ, (void*)buffer, count) )
	{	return -EFAULT;
	}
    if( down_interruptible( &scr->scrSem ) )
    {
        return -ERESTARTSYS;
    }
   	//if (!scr->writing)
    if( !atomic_read( &scr->wr_busy ) )
	{	
        int alen = count > MAX_DATA_LEN ? MAX_DATA_LEN : count;
		int rval;
		copy_from_user(scr->output, buffer, alen);
        atomic_set( &scr->wr_busy, 1 );
		if ((rval = DoSendData(scr, alen)))
		{	
            atomic_set( &scr->wr_busy, 0 );
            up( &scr->scrSem );
            KdPrint( DBG_WRITE, ("Can not init writing") );
			return rval;
		}
		buffer += alen;
		count -= alen;
		written += alen;
	}
	KdPrint( DBG_WRITE, ("(%d) write: wp: %d, rp: %d\n", __LINE__, scr->obuf_wp, scr->obuf_rp) );
	{	
        int obsz = scr->obuf_wp - scr->obuf_rp;
		if (obsz < 0)
		{	obsz += OBUF_SIZE;
		}
		if (obsz + count > OBUF_SIZE - 1)
		{	
            up( &scr->scrSem );
			//interruptible_sleep_on(&scr->wr_wait_q);
            if( wait_event_interruptible( scr->wr_wait_q, 
            ( ( scr->obuf_wp - scr->obuf_rp + count ) > ( OBUF_SIZE-1)) ) )
            {
                return -ERESTARTSYS;
            }
			if( down_interruptible( &scr->scrSem ) )
            {
               return -ERESTARTSYS;
            }
        }
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
	up( &scr->scrSem );
	
	return written;
}

static ssize_t  read_tscreen(struct file * file, 
                             char *dest,
                             size_t size, 
                             loff_t *ppos)
{	
	struct scr_usb_data *scr;
	int is_nblk = file->f_flags & O_NONBLOCK;
	int totalrd = 0;
	int actualrd, datalen;
	KdPrint( DBG_READ, ("Enter read_tscreen\n") );
	scr = file->private_data;
    if( !scr )
    {
        KdPrint( DBG_READ, (" Levae read_tscreen : No scr data\n"));
        return -ENODEV;
    }
	if( down_interruptible( &scr->scrSem ) )
    {
        return -ERESTARTSYS;
    }
	if (!scr->present)
	{	
        KdPrint( DBG_READ, (" Levae read_tscreen : No scr data\n"));
        up( &scr->scrSem );
        return -ENODEV;
	}
	file->f_dentry->d_inode->i_atime = CURRENT_TIME; /* Update the atime of inode */

	if ( VERIFY_AREA(VERIFY_WRITE, dest, size) )
	{	
        KdPrint( DBG_READ, (" Verify_area failure\n"));
        up( &scr->scrSem );
        return -EFAULT;
	}
    if( ( scr->ibuf_rp == scr->ibuf_wp ) )
    {
        up( &scr->scrSem ); // release the semaphore
        if( is_nblk )
        {
            KdPrint( DBG_READ,(" Leave read_tscreen:None blocking Read\n"));
            return -EAGAIN;
        }
        KdPrint( DBG_READ,("Read going to sleep\n"));
        if( wait_event_interruptible( scr->rd_wait_q, 
                                      (scr->ibuf_rp != scr->ibuf_wp ) ) )
        {
            KdPrint(DBG_READ,(" Leave read_tscreen:Wait event failure\n"));
            return -ERESTARTSYS;
        }
	    if( down_interruptible ( &scr->scrSem ) )
        {
            return -ERESTARTSYS;
        }
        KdPrint(DBG_READ,("Away from sleep\n"));
    }
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
	up(&scr->scrSem );
    KdPrint(DBG_READ,("Read finished count=%d\n", totalrd ));
	return totalrd;	
}

static unsigned int poll_tscreen(struct file *file, struct poll_table_struct *wait)
{	struct scr_usb_data *scr;
	int rval = 0;

	KdPrint(DBG_POLL,("poll called\n"));

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
{	
   struct scr_usb_data *scr;
	int rval = -EINVAL;

	//SALUTE("ioctl called");

    KdPrint( DBG_IOCTL, ("Enter ioctl_tscreen\n"));
	if ( !(scr=file->private_data) )
	{	return rval;
	}
	switch (cmd)
	{
	case SCR_IOCTL_SHOW_STATUS:
	{	ModuleStatus stat;
		rval = VERIFY_AREA(VERIFY_WRITE, (void*)arg, sizeof(ModuleStatus));
		if ( !rval )
		{	copy_to_user((ModuleStatus*)arg, &stat, sizeof(ModuleStatus));
		}
		break;
	}
	}
    KdPrint( DBG_IOCTL, ("Leave ioctl_tscreen\n"));
	return rval;
}
#endif
static struct usb_device_id tk_table[] = {
    { USB_DEVICE( 0x1234, 0x0001 )},
    { USB_DEVICE( 0x1234, 0x0002 )},
    { USB_DEVICE( 0x0EEF, 0x0001 )}, 
    { USB_DEVICE( 0x0EEF, 0x0002 )},
    { USB_DEVICE( 0x3823, 0x0001 )},
    { USB_DEVICE( 0x3823, 0x0002 )}
};

MODULE_DEVICE_TABLE( usb, tk_table );

static struct file_operations usb_tscreen_fops = {
                                .owner = THIS_MODULE,
	                            .read  = read_tscreen,
	                            .write = write_tscreen,
	                            .poll =	poll_tscreen,
	                            .open =	open_tscreen,
	                            .release = close_tscreen,
	                            .fasync	= fasync_tscreen,
	                        #ifdef IOCTL_SHOW_STATUS
	                            .ioctl = ioctl_tscreen
	                        #endif
                                };

static struct usb_class_driver tkClass =
{
    .name = "usb/tkpanel%d",
    .fops = &usb_tscreen_fops,
    //.mode = S_IFCHR| S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH,
    .minor_base = SCR_BASE_MNR
};

/*
static void *probe_tscreen(struct usb_device *dev, unsigned int ifnum
		     ,const struct usb_device_id *id )
*/
static int probe_tscreen( struct usb_interface *If, const struct usb_device_id *id )
{
	//struct scr_usb_data *scr;
    struct usb_device *udev = interface_to_usbdev( If );
    struct scr_usb_data *scr = NULL;
    int scr_minor;
    int retval = -ENOMEM;
	//kdev_t scr_minor;

	KdPrint( DBG_USB,("probe_tscreen called\n"));
    if( (udev->descriptor.idVendor != 0x1234 &&
         udev->descriptor.idVendor != 0x3823 &&
         udev->descriptor.idVendor != 0x0EEF ) )
    {
        return -ENODEV;
    }
    if( ( udev->descriptor.idProduct != 0x0001 &&
          udev->descriptor.idProduct != 0x0002 ) )
    {
        return -ENODEV;
    }
    KdPrint( DBG_USB,("Probe tscreen: minor=%d\n", If->minor ));
    if( If->minor > SCR_BASE_MNR )
    {
        scr_minor = If->minor- SCR_BASE_MNR;
    }
    else
    scr_minor = If->minor;
	KdPrint( DBG_USB,("probe_tscreen: (new panel) Allocated minor:%d\n", scr_minor));
    if (!(scr = kmalloc (sizeof (struct scr_usb_data), GFP_KERNEL))) 
    {
			KdPrint( DBG_USB,("probe_tscreen: Out of memory."));
			return -ENOMEM;
	}

	memset (scr, 0, sizeof(struct scr_usb_data));
	KdPrint ( DBG_USB,("probe_tscreen(%d): Address of scr:%p\n", scr_minor, scr));
    scr->scr_dev = udev;
    scr->If = If;
    scr->rdUrb = usb_alloc_urb( 0, GFP_KERNEL );
    if( !scr->rdUrb  )
    {
        KdPrint( DBG_USB,("probe_tscreen: Out of memory\n"));
        kfree( scr );
        return -ENOMEM;
    }
    scr->wrUrb = usb_alloc_urb( 0, GFP_KERNEL );
    if(!scr->wrUrb )
    {
        KdPrint( DBG_USB,(" probe_tscreen: Out Of memory\n"));
        usb_free_urb( scr->rdUrb );
        kfree( scr );
        return -ENOMEM;
    }
    init_MUTEX( &scr->scrSem );
	/* Ok, now initialize all the relevant values */
	if (!(scr->obuf = (char *)kmalloc(OBUF_SIZE, GFP_KERNEL))) 
    {
			KdPrint(DBG_USB,("probe_tscreen(%d): Not enough memory for the output buffer.", scr_minor));
            usb_free_urb( scr->rdUrb );
            usb_free_urb( scr->wrUrb );
			kfree(scr);
			return -ENOMEM;
	}
	KdPrint(DBG_USB,("probe_tscreen(%d): obuf address:%p\n", scr_minor, scr->obuf));
	scr->obuf_rp = scr->obuf_wp = 0;
	if (!(scr->ibuf = (char *)kmalloc(IBUF_SIZE, GFP_KERNEL))) 
    {
			KdPrint(DBG_USB,("probe_tscreen(%d): Not enough memory for the input buffer.", scr_minor));
            usb_free_urb( scr->rdUrb );
            usb_free_urb( scr->wrUrb );
			kfree(scr->obuf);
			kfree(scr);
			return -ENOMEM;
	}
	KdPrint(DBG_USB,("probe_tscreen(%d): ibuf address:%p\n", scr_minor, scr->ibuf));
	scr->ibuf_wp = scr->ibuf_rp = 0;
	scr->present = 1;
	scr->scr_dev = udev;
	scr->scr_minor = scr_minor;
	scr->closing = 0;
	scr->opencnt = 0;
	scr->writecnt = 0;
	scr->readcnt = 0;
	scr->fasync = NULL;
	init_waitqueue_head(&scr->rd_wait_q);
	init_waitqueue_head(&scr->wr_wait_q);
	init_waitqueue_head(&scr->op_wait_q);
    atomic_set( &scr->rd_busy, 0 );
    atomic_set( &scr->wr_busy, 0 );
    init_completion( &scr->rd_done );
    init_completion( &scr->wr_done );
        /* Add by Alex for new USB core*/
    KdPrint(DBG_USB,(" Set Interface data  \n"));
    usb_set_intfdata( If, scr );
    KdPrint(DBG_USB,(" Register USB device \n"));
    retval = usb_register_dev( If, &tkClass );
    KdPrint(DBG_USB,(" Register USB device Done Retval =%d\n", retval ));
    if( retval )
    {
          usb_set_intfdata( If, NULL );
          usb_free_urb( scr->rdUrb );
          usb_free_urb( scr->wrUrb );
          kfree( scr->ibuf );
          kfree( scr->obuf );
          kfree( scr );
          KdPrint(DBG_USB,(" probe_tscreen failure\n"));
    }
    else
    {
            scr->scr_minor = scr_minor;
            KdPrint(DBG_USB,(" probe_tscreen successful\n"));
    }
	return retval;
}

static void disconnect_tscreen( struct usb_interface *If )
{
    
    struct scr_usb_data *scr;
    int scr_minor;
    KdPrint(DBG_USB,("disconnect_tscreen called %p\n", If));
	down( &disconnect_sem );
    scr = usb_get_intfdata( If );
    if( scr )
    {
        usb_set_intfdata( If, NULL );
        scr_minor = scr->scr_minor;
        if( down_interruptible (&scr->scrSem) )
        {
            return;// -ERESTARTSYS;
        }
    	scr->present = 0;
        usb_deregister_dev( If, &tkClass );
        if( atomic_read( &scr->rd_busy ) )
        {
            usb_unlink_urb( scr->rdUrb );
            wait_for_completion( &scr->rd_done );
        }
        if( atomic_read( &scr->wr_busy ) )
        {
            usb_unlink_urb( scr->wrUrb );
            wait_for_completion( &scr->wr_done );
        }
    	if (scr->readcnt)
    	{	
            KdPrint(DBG_USB,("disconnect_tscreen(%d): Unlinking IRQ URB", scr->scr_minor));
    		wake_up_interruptible(&scr->rd_wait_q);
    	}
    	if (scr->closing)
    	{	
            wake_up_interruptible(&scr->wr_wait_q);
    	}
        KdPrint(DBG_USB,("after disconnect:  rcnt: %d, wcnt %d, ocnt %d\n",
    			  scr->readcnt, scr->writecnt, scr->opencnt));
        if (!scr->opencnt)// If no one open this device
        {	
            KdPrint(DBG_USB,("disconnect_tscreen: De-allocating minor:%d\n", scr->scr_minor));
            delete_tscreen( scr );
    	}
        else
        {
            up(&scr->scrSem);
        }
    }
    up( &disconnect_sem );

}




static struct usb_driver tscreen_driver = {
                                            //.owner = THIS_MODULE,
                                            .name = IDcat(PRJNAME, _USBMODULE),
                                            .probe = probe_tscreen,
                                            .disconnect = disconnect_tscreen,
                                            .id_table = tk_table };

void __exit usb_tscreen_exit(void)
{	
    usb_deregister(&tscreen_driver);
}

int __init usb_tscreen_init (void)
{
    init_MUTEX( &disconnect_sem );
    if (usb_register(&tscreen_driver) < 0)
    	return -1;
	KdPrint(DBG_USB,("USB TouchScreen support registered.\n"));
	return 0;
}

module_init(usb_tscreen_init);
module_exit(usb_tscreen_exit);
