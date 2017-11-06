/* tab size: 4 */

/**************************************
 *   Basic Driver Operation
 **************************************/

#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "tpanel.h"

static int DriverPID = -1;
static int DriverStop = 0;

void ContDriver()
{	if (DriverPID >= 0)
	{	kill(DriverPID, SIGCONT);
		DriverPID = -1;
	}
}


static int WaitUntilDriverCont(int pid, int timeout)
{	int cnt, done;

	for (done=cnt=0; ; cnt++)
	{	FILE *fp;
		char name[100];
		sprintf(name, "/proc/%d/stat", pid);
		if ( (fp=fopen(name, "r")) )
		{	char buf[3][300];
			if ( fscanf(fp, "%s%s%s", buf[0], buf[1], buf[3]) == 3 
				&& buf[3][0] != 'T' )
			{	done = 1;
			}
			fclose(fp);
		}
		if (done)
		{	return 0;
		}
		if (cnt <= timeout)
		{	return 1;
		}
		sleep(1); 
	}
}

void StopDriver()
{	FILE *fp;

	if ( (fp=fopen(IDcat(PRJNAME, _LOCK), "r")) )
	{	int pid;
	
		if ( fscanf(fp, "%d", &pid) && !kill(pid, 0) )
		{	WaitUntilDriverCont(pid, 3);
			DriverPID = pid;
			if (!DriverStop)
			{	kill(DriverPID, SIGSTOP);
				atexit(ContDriver);
				DriverStop = 1;
			}
		}
		fclose(fp);
	}
}
