
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
#include "../include/config.h"
//#define MONITOR_DEVICE_LIST
// or to monitor usb directory with polling

#define TIME_TO_WAKEUP_IN_SEC	1 //.25

#define DEBUG 1

#define USBFS_PATH		"/proc/bus/usb/"
#define USB_DEVICES		"/proc/bus/usb/devices"		
#define DBG
int DaemonReset = 1;
struct stat Oldstate = {0};
void TakeAction()
{	
	//printf(" Reset now \n");
	system("/usr/bin/tpaneld -f");
	DaemonReset = 0;
}

void handler(int dummy)
{	/*if (dummy == SIGHUP)
	{	DBG("reset daemon");
		DaemonReset = 1;
	}*/
	struct stat statbuf;
	//printf(" handler now \n");
	if( !stat( USB_DEVICES, &statbuf ) )
	{
		if( statbuf.st_mtime != Oldstate.st_mtime )
		{
			//printf(" file updated! It needs reset daemon now \n");
		        DaemonReset = 1;
			Oldstate = statbuf;
		}
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
		{	//ReadCFG();
			//printf(" Take action now \n");
			TakeAction();	
		}
		pause();
	}
	return 0;
}

