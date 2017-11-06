
#ifndef _EGTOUCH_DBG_H_ // {
#define _EGTOUCH_DBG_H_

//#define VERBOSE
#ifdef VERBOSE
	#define MESSAGE_SWITCH
//	#define IOCTL_CMD_DBG
	#define FUNC_CALLED_DBG
//	#define TIMER_DBG	
	#define MBI_DBG
	#define SEND_DATA_DBG
	#define THROUGHPUT_DBG
	#define URB_STATUS_DBG
#endif

//#define IOCTL_SHOW_STATUS

#ifdef VERBOSE
	#ifdef MESSAGE_SWITCH
		static int MessageOn = 1;
		#define MSG(head, arg...)		\
			(MessageOn ? (printk(head __FILE__ ": " arg)): 0)
	#else
		#define MSG(head, arg...)		printk(head __FILE__ ": " arg)
	#endif
#else
	#define MSG(head, arg...)
#endif

#ifdef FUNC_CALLED_DBG
	#define SALUTE(arg...)	MSG("", arg)
#else
	#define SALUTE(arg...)
#endif	

#ifdef SERIALIZE_DBG
	#define ENTER()				
	#define LEAVE()
#else
	#define ENTER()
	#define LEAVE()
#endif	

#if defined(TIMER_DBG) && defined(USE_TIMER) && defined(VERBOSE)
	#define DUMP_TIMER()											\
	{	register int i;												\
		MSG("global timer %d\n", sunixTimer_on);					\
		for (i=0; i<sunix_numBoards; i++)							\
		{	printk("[brd %d: %02X]", i, sunix_boards[i].fwLoading);	\
		}															\
		printk("\n");												\
	}
#else
	#define DUMP_TIMER()
#endif

#ifdef VERBOSE
	#define DBG(arg...)		MSG("<D>", arg)
	#define INFO(arg...)	MSG("<I>", arg)
	#define ERR(arg...)		MSG("<E>", arg)
#else
	#define DBG(arg...)
	#define INFO(arg...)
	#define ERR(arg...)		printk(KERN_ERR arg)
#endif


typedef struct
{	int xxx;
}	ModuleStatus;

#endif // }


