
/* Tab: 4 */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

//#define MONITOR_DEVICE_LIST
// or to monitor usb directory with polling

#define TIME_TO_WAKEUP_IN_SEC	.25

#define DEBUG 1

#define USBFS_PATH		"/proc/bus/usb/"
#define USBDEV_PATTERN	"[0-9]??"

#if DEBUG
	#define DBG(arg...)		(printf(arg),puts(""))
	#define ERR(arg...)				\
	{	char __buf[1000];			\
		sprintf(__buf, arg);		\
		perror(__buf);				\
	}
#else
	#define DBG(arg...)		
	#define ERR(arg...)		
#endif

#include "config.h"

typedef struct Device
{	int tag;
	unsigned int vendor_id;
	unsigned int product_id;
	char *action;
	char *path;
	struct Device *next;
}	Device;

int DaemonReset = 0;

FILE *cfgFP = NULL;
int DeviceCnt = 0;
Device *DeviceList = NULL, *DeviceCFGed = NULL;

char *_new_string(char *src)
{	int len = strlen(src);
	char *p = malloc(len+1);
	if (p)
	{	strcpy(p, src);
		if (len && p[len-1] == '\n')
		{	p[len-1] = '\0';
		}
	}
	return p;
}

void _remove_none_digit(char *src)
{	for (; *src; src++)
	{	if ( !isdigit(*src) )
		{	*src = ' ';
		}
	}
}

#if DEBUG
void _dump_list(Device *head)
{	if (head)
	{	DBG("T(%d) Vid: %04X, Pid %04X, Act: {%s}, Path: {%s}",
			head->tag, head->vendor_id, head->product_id, head->action, head->path);
		_dump_list(head->next);
	}
}
#else
	#define _dump_list(x)
#endif

void _destroy_list(Device *head)
{	if (head)
	{	_destroy_list(head->next);
		if (head->action)
		{	free(head->action);
		}
		if (head->path)
		{	free(head->path);
		}
		free(head);
	}
}

void _set_tag_field(Device *list, int value)
{	for (; list; list = list->next)
	{	list->tag = value;
	} 
}

void _set_tag_if_match(Device *list, Device *sample, int value)
{	for (; list; list = list->next)
	{	if (list->vendor_id == sample->vendor_id
			&& list->product_id == sample->product_id
			#ifndef MONITOR_DEVICE_LIST
				&& list->path
				&& sample->path
				&& !strcmp(list->path, sample->path)
			#endif
			)
		{	list->tag = value;
		}
	}
}

int _is_comment(char *line)
{	for (; *line && isspace(*line); line++) {}
	return (*line == '#');
}

int _is_keyword(char *src, char *keyword)
{	char word[200];
	return (sscanf(src, "%s", word) && !strcasecmp(word, keyword));
}

int _atoi_ok(char *src, int *rval)
{	char *endp;
	*rval = strtol(src, &endp, 0);
	return (endp > src);
}

int _read_setting_from_line(char *line, int *vid, int *pid, char **action)
{	char *equal, *comma1, *comma2;

	if (_is_comment(line))
	{	return -1;
	}	
	comma1 = comma2 = NULL;
	equal = strchr(line, '=');
	if (equal)
	{	comma1 = strchr(equal+1, ',');
	}
	if (comma1)
	{	comma2 = strchr(comma1+1, ',');
	}
	if ( !equal || !comma1 || !comma2 )
	{	return -1;
	} 
	*equal = *comma1 = *comma2 = '\0';
	if ( _is_keyword(line, "DEVICE") 
		&& _atoi_ok(equal+1, vid)
		&& _atoi_ok(comma1+1, pid) )
	{	char *p;
		for (p = comma2+1; *p && isspace(*p); p++) {}
		*action = p;
		return 0;
	}else
	{	return -1;
	}
}

/*
**	Read $PNP_CONFIG, and build list of devices configured (DeviceCFGed).
**
**	(this function always resets DeviceCFGed)
*/
int ReadCFG()
{	FILE *fp;

	if ( (fp = fopen(PNP_CONFIG, "r")) == NULL )
	{	if ( (fp = fopen(PNP_CONFIG, "w+")) == NULL )
		{	ERR("fopen(\"" PNP_CONFIG "\")");
			return -1;
		}else
		{	fprintf(fp, "%s\n", DefaultUSBpnpdConf);
			rewind(fp);
		}
	}
	
	{	char buf[1000];
		
		if (DeviceCFGed)
		{	_destroy_list(DeviceCFGed);
			DeviceCFGed = NULL;
		}
		while (fgets(buf, sizeof buf, fp))
		{	int vid, pid;
			char *action;
		
			if ( !_read_setting_from_line(buf, &vid, &pid, &action) )
			{	Device *newNode = malloc(sizeof(Device));
				if (!newNode)
				{	ERR("malloc()");
				}else
				{	newNode->tag		= 0;
					newNode->vendor_id	= vid;
					newNode->product_id	= pid;
					newNode->action		= _new_string(action);
					newNode->path		= NULL;
					newNode->next		= DeviceCFGed;
					DeviceCFGed			= newNode;
				}
			}
		}
		DBG("(*) DeviceCFGed:");
		_dump_list(DeviceCFGed);
		fclose(fp);
		return 0;
	}
}


/*
**	Read $DEV_LIST, and build list of devices found.
**
**	(this function always resets DeviceList and DeviceCnt)
*/

#ifdef MONITOR_DEVICE_LIST /* { */

int UpdateDeviceList()
{	char buf[500];

	rewind(cfgFP);
	DeviceList = NULL;
	DeviceCnt = 0;
	while ( fgets(buf, sizeof(buf), cfgFP) )
	{	if (buf[0] == 'P')
		{	int vendor, product;
			char num[2][30] = { "0x", "0x"};
			char *equal1, *equal2;
			
			if ( (equal1=strchr(buf, '=')) && sscanf(equal1+1, "%s", num[0]+2)
				&& (equal2=strchr(equal1+1, '=')) && sscanf(equal2+1, "%s", num[1]+2) )
			{	if ( _atoi_ok(num[0], &vendor) && _atoi_ok(num[1], &product)
					&& (vendor || product) )
				{	Device *newNode = malloc(sizeof(Device));
					if (!newNode)
					{	ERR("malloc()");
					}else
					{	newNode->tag		= 0;// used in TakeAction() only
						newNode->vendor_id	= vendor;
						newNode->product_id	= product;
						newNode->action		= NULL;
						newNode->path		= NULL;
						newNode->next		= DeviceList;
						DeviceList			= newNode;
						DeviceCnt++;
					}
				}
			}
		}
	}
	DBG("(*) DeviceList:");
	_dump_list(DeviceList);
	return 0;
}

#else /* } { ! MONITOR_DEVICE_LIST */

void DumpGlob(glob_t *gdata)
{	char **pathv;
	for (pathv = gdata->gl_pathv; pathv[0]; pathv++ )
	{	puts(pathv[0]);
	}
}

int IsDirectory(char *path)
{	int len = strlen(path);
	return (len && path[len-1] == '/');
}

int GlobRecursively(char *path, char *pattern, glob_t *gbuf)
{	
	char fullpath[500];	
	int idx, start = gbuf->gl_pathc, end;
	int flag = GLOB_MARK;

	if ( start )	flag |= GLOB_APPEND;

	sprintf(fullpath, "%s%s", path, pattern);
	if ( glob(fullpath, flag, NULL, gbuf) )
	{	return -1;
	}
	end = gbuf->gl_pathc;
	
	for ( idx = start; idx < end; idx++ )
	{	char *path = gbuf->gl_pathv[idx];
		if ( IsDirectory(path) )
		{	if ( GlobRecursively(path, pattern, gbuf) )
			{	return -1;
			}
		}
	}
	return 0;
}

#include "linux/usb.h"

int ReadUSBdeviceInfo(char *path, Device *output)
{	int fd;
	int rval = -1;
	
	if ( (fd = open(path, O_NONBLOCK)) >= 0 )
	{	char buf[100];
		if ( read(fd, buf, sizeof buf) > 0 )
		{	
			/*
			struct usb_device_descriptor *dp = (struct usb_device_descriptor*)buf;
			output->vendor_id = dp->idVendor;
			output->product_id = dp->idProduct;
			*/
			output->vendor_id = *(unsigned short*)(buf+8);
			output->product_id = *(unsigned short*)(buf+10);
			rval = 0;
		}
		close(fd);
	}
	return rval;
}

int UpdateDeviceList()
{	
	char **pathv;
	glob_t gbuf;
	
	DeviceList = NULL;
	DeviceCnt = 0;

	chdir(USBFS_PATH);
	gbuf.gl_pathc = 0;
	if ( GlobRecursively("", USBDEV_PATTERN, &gbuf) )
		return -1;
		
	for ( pathv = gbuf.gl_pathv; pathv[0]; pathv++ )
	{	Device info;
		if ( !IsDirectory(pathv[0]) 
			&& !ReadUSBdeviceInfo(pathv[0], &info)
			&& (info.vendor_id | info.product_id) )
		{	Device *newNode = malloc(sizeof(Device));
			if (!newNode)
			{	ERR("malloc()");
			}else
			{	*newNode			= info;	// copy vid and pid
				newNode->tag		= 0;
				newNode->action		= NULL;
				newNode->path		= _new_string(pathv[0]);
				newNode->next		= DeviceList;
				DeviceList			= newNode;
				DeviceCnt++;
			}
		}
	}	
	globfree(&gbuf);

	DBG("(*) DeviceList:");
	_dump_list(DeviceList);
	return 0;
}

#endif /* ! MONITOR_DEVICE_LIST } */

void DoTakeAction(Device *current, Device *cfg_list)
{	Device *match = NULL;
	for (; cfg_list; cfg_list = cfg_list->next)
	{	if (cfg_list->vendor_id == current->vendor_id
			&& cfg_list->product_id == current->product_id)
		{	match = cfg_list;
		}
	}
	if (match && match->action)
	{	DBG("exec {%s} for device Vid: 0x%04X, Pid 0x%04X",
			match->action, match->vendor_id, match->product_id);
		system(match->action);
	}else
	{	DBG("no idea about device Vid: 0x%04X, Pid 0x%04X",
			current->vendor_id, current->product_id);
	}
}

void TakeAction()
{	Device *old_list = DeviceList;
	int old_cnt = DeviceCnt;
	#define NOT_FOUND	0
	#define FOUND		1

	UpdateDeviceList();
	
	if (DeviceCnt >= old_cnt)
	{	Device *ptr;
		#ifdef MONITOR_DEVICE_LIST	
			if (DeviceCnt == old_cnt)
			{	// SIGHUP caught
				_destroy_list(old_list);
				old_list = NULL;
			}
		#endif
		_set_tag_field(DeviceList, NOT_FOUND);
		for (ptr=old_list; ptr; ptr = ptr->next)
		{	_set_tag_if_match(DeviceList, ptr, FOUND);
		}
		for (ptr=DeviceList; ptr; ptr = ptr->next)
		{	if (ptr->tag == NOT_FOUND)
			{	DBG("new device: Vid 0x%04X, Pid 0x%04X",
					ptr->vendor_id, ptr->product_id);
				DoTakeAction(ptr, DeviceCFGed);
			}
		}	
	}else
	{	DBG("device removed");
	}
	_destroy_list(old_list);
}

void handler(int dummy)
{	if (dummy == SIGHUP)
	{	DBG("reset daemon");
		DaemonReset = 1;
	}
}

void _background(int argc)
{	
	if ( argc < 2 ) daemon(0,0);
}

void _update_lock()
{	FILE *fp;
	if ( (fp=fopen(PRG_LOCK, "w")) )
	{	fprintf(fp, "%d\n", getpid());
		fclose(fp);
	}
}

int _is_dup(char *name)
{	FILE *fp;
	int pid;
	
	fp = fopen(PRG_LOCK, "r");
	if (fp && fscanf(fp, "%d", &pid) && !kill(pid, 0))
	{	fclose(fp);
		DBG("already exists");
		return 1;
	}else if(fp)
	{	fclose(fp);
	}
	return 0;
}

int main(int argc, char **argv)
{
#ifdef MONITOR_DEVICE_LIST
	int fd;
	int rval;
	fd_set fds;
#else

#endif

	char *prgname;

	if ( !(prgname = strrchr(argv[0], '/')) )
	{	prgname = argv[0];
	}
	if ( _is_dup(prgname) )
	{	char cmd[1000];
		if (argc > 1)
		{	sprintf(cmd, "killall -HUP %s", prgname);
			system(cmd);
		}
		return 0;
	}
	_background(argc);
	_update_lock();
	signal(SIGHUP, handler);
	if (ReadCFG())
	{	DBG("ReadCFG() failed");
		return -1;
	}
	
#ifdef MONITOR_DEVICE_LIST	/* { */
	
	if ( (fd = open(DEV_LIST, O_RDONLY)) < 0 ) 
	{	ERR("open(" DEV_LIST ")");
		return -1;
	}
	if ( (cfgFP = fdopen(fd, "r")) == NULL )
	{	ERR("fdopen(" DEV_LIST ")");
		close(fd);
		return -1;
	}
	while (1)
	{	FD_ZERO(&fds);
		FD_SET(fd, &fds);
		
		rval = select(fd+1, &fds, NULL, NULL, NULL);
		if (rval == 1)
		{	TakeAction();
		}else if (errno != EINTR)
		{	DBG("signal caught");
			break;
		}else if (DaemonReset)
		{	ReadCFG();
			TakeAction();
		}
	}
	close(fd);
	
#else /* } { !MONITOR_DEVICE_LIST */
	
	// init polling clock and handler	
	signal(SIGALRM, handler);
	{   struct itimerval timer;
		long sec = TIME_TO_WAKEUP_IN_SEC;
		long usec = (TIME_TO_WAKEUP_IN_SEC-sec) * 1.0e6;
		
		timer.it_value.tv_sec   = sec;
		timer.it_value.tv_usec  = usec;
		timer.it_interval = timer.it_value;
		setitimer(ITIMER_REAL, &timer, NULL);
	}

	while (1)
	{	if (DaemonReset)
		{	ReadCFG();
		}
		TakeAction();
		pause();
	}
	
#endif /* !MONITOR_DEVICE_LIST } */

	return 0;
}

