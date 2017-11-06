
#ifndef _EEPROM_C_
#define _EEPROM_C_

//#define EEPROM_DEBUG

#ifdef EEPROM_DEBUG
	#define READ_DETAIL
#endif

// delay in millisecond
#define READ_DELAY			0
#define WRITE_DELAY			0

#include "tpanel.h"
#include "eeprom.h"

#include "device.c"

void FixEEPROM(EEPROM eeprom)
{	int i;
	EEPROM_UNIT sum;

	for (sum=i=0; i<EEPROM_SIZE-1; i++)
	{	sum += eeprom[i];
	}
	eeprom[EPR_CheckSum] = sum + CHECK_SUM_BASE;
}


int CheckEEPROM(EEPROM eeprom)
{	int i;
	EEPROM_UNIT sum;

	for (sum=i=0; i<EEPROM_SIZE-1; i++)
	{	sum += eeprom[i];
	}
	return ((sum + CHECK_SUM_BASE) != eeprom[EPR_CheckSum]);
}

int _ReadEEPROM(int fd, int addr, EEPROM_UNIT *datp)
{
	#define READ_RETRY		5
	int retry = READ_RETRY;

	#ifdef READ_DETAIL
		printf("Try to read addr %d:\n", addr);
	#endif
	for ( ; retry >= 0; retry--)
	{	int cmd = H2D_GETEEPROM;
		char buf[MaxCmdLen], *cmdbuf = buf;
		int cmdlen = 1;
		
		#ifdef READ_DETAIL
			printf("\t(%d) ", READ_RETRY - retry);
		#endif
		buf[0] = addr;
		if ( !SendAndWait(fd, &cmd, &cmdbuf, &cmdlen) )
		{	unsigned char *ptr = cmdbuf;
			
			#ifdef READ_DETAIL
				printf("OK cmd %c addr %d\n", cmd, cmdbuf[0]);
			#endif
			if ( cmdbuf[0] < addr )
			{	char buf[100];
				while ( ReadFunc(fd, buf, sizeof buf) > 0 ){;}
				continue;
			}
			if ( cmd == D2H_EEPROM )
			{	*datp = ptr[1] + ((int)ptr[2] << 8);
			//	*datp = *(EEPROM_UNIT*)(ptr+1);  // architecture independence
				return 0;
			}else if ( cmd == D2H_EEPROMFIXED )
			{	*datp = ptr[1] + ((int)ptr[2] << 6) + ((int)ptr[3] << 12);
				return 0;
			}
		}
		#ifdef READ_DETAIL
			printf("timeout\n");
		#endif
	}
	return 1;
}

int ReadEEPROM(int fd, EEPROM eeprom)
{	int i;
	
	SendSimpleCommand(fd, H2D_AVOIDCTRLCHAR);

	#ifdef EEPROM_DEBUG
		for (i=0; i<EEPROM_SIZE; i++)
		{	eeprom[i] = 0xEEEE;
		}
	#endif

	for (i=0; i<EEPROM_SIZE; i++)
	{	if ( _ReadEEPROM(fd, i, eeprom+i) )
		{	break;
		}
		MilliSecDelay(READ_DELAY);
	}
	
	#ifdef EEPROM_DEBUG
	{	int i;
		printf("ReadEEPROM() Result:\n");
		for (i=0; i<EEPROM_SIZE; i++)
		{	printf("[%04X] ", eeprom[i]);
			if ( !((i+1)%8) )
			{	printf("\n");
			}
		}
	}
	#endif
	
	return i < EEPROM_SIZE ? 1 : CheckEEPROM(eeprom);
}


int _WriteEEPROM(int fd, EEPROM eeprom, int start, int end)
{	int i;
	char buf[10];
	
	while ( ReadFunc(fd, buf, sizeof(buf)) ) {;}
	
	if (start < 0)
	{	start = 0;
	}
	if (end > EEPROM_SIZE-1)
	{	end = EEPROM_SIZE-1;
	}
	
//	for (i=end; i>=start; i--)
	for (i=start; i<=end; i++)
	{	
		// !!! USB BUG?? 
		if ( EPR_LiftDelay < i && i < EPR_Flags - 1 ) continue;
		// !!!!!!!!!!!!!!!!!!!!!
		
		buf[0] = i;
//		*(EEPROM_UNIT*)(buf+1) = eeprom[i];
		buf[1] = eeprom[i] & 0x00FF;	// architecture independence
		buf[2] = eeprom[i] >> 8;

		// SendCommand(fd, H2D_SETEEPROM, buf, 1+sizeof(EEPROM_UNIT));
		{	int cmd = H2D_SETEEPROM;
			char *cmdbuf = buf;
			int cmdlen = 1 + sizeof(EEPROM_UNIT);
			if ( SendAndWait(fd, &cmd, &cmdbuf, &cmdlen) ||
				cmd != D2H_SETEEPROMOK )
			{	EEPROM_UNIT tmp;
				MilliSecDelay(WRITE_DELAY);
				if ( _ReadEEPROM(fd, i, &tmp) || tmp != eeprom[i] )
				{	return 1;
				}
			}
		}
	}
	MilliSecDelay(WRITE_DELAY);
	
	#ifdef EEPROM_DEBUG
		printf("WriteEEPROM(%d~%d) expect:\n", start, end);
		for (i=0; i<EEPROM_SIZE; i++)
		{	printf("[%04X] ", eeprom[i]);
			if ( !((i+1)%8) )
			{	printf("\n");
			}
		}
	#endif

	return 0;
}

int LoadEEPROM(char *filename, EEPROM eeprom)
{
	FILE *fp;
	int num;
	
	if ( (fp = fopen(filename, "rb")) == NULL )
	{	return -1;
	}
	
	num = fread(eeprom, sizeof(EEPROM_UNIT), EEPROM_SIZE, fp);
	fclose(fp);
	if ( num == EEPROM_SIZE )
	{	return CheckEEPROM(eeprom);
	}else
	{	return -1;
	}

}

int SaveEEPROM(char *filename, EEPROM eeprom)
{
	FILE *fp;
	int num;

	if ( (fp = fopen(filename, "wb")) == NULL )
	{	return -1;
	}
	num = fwrite(eeprom, sizeof(EEPROM_UNIT), EEPROM_SIZE, fp);
	fclose(fp);
	return -(num == EEPROM_SIZE);

}

void CopyEEPROM(EEPROM dest, EEPROM src)
{	int i;
	for ( i=0; i<EEPROM_SIZE; i++)
		dest[i] = src[i];
}

int CachedReadEEPROM(char *romfile, int fd, EEPROM eeprom)
{	EEPROM temp;
	int rval = 0;
	
	if ( !LoadEEPROM(romfile, eeprom) )
	{	temp[EPR_CheckSum] = ~eeprom[EPR_CheckSum];
		if ( _ReadEEPROM(fd, EPR_CheckSum, temp + EPR_CheckSum)
			|| temp[EPR_CheckSum] != eeprom[EPR_CheckSum] )
		{	if ( !(rval = ReadEEPROM(fd, temp)) )
			{	CopyEEPROM(eeprom, temp);
				SaveEEPROM(romfile, temp);
			}
		}
	}else if ( !(rval = ReadEEPROM(fd, eeprom)) )
	{	SaveEEPROM(romfile, eeprom);
	}
	return rval;
}

#endif

