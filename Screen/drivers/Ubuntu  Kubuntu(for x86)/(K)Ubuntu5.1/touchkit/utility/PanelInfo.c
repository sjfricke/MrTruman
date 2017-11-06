
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "tpanel.h"
#include "configSTR.h"
#include "configINT.h"

#include "device.c"
#include "driver.c"
#include "eeprom.c"     

#define GetPanelInfoHelp										\
"Usage: GetPanelInfo device [options]\n"						\
"Options:\n"													\
"  -f           Fix EEPROM Checksum\n"							\
"  -v           Display Firmware Version\n"						\
"  -t           Display Controller Type\n"						\
"  -p           List Parameters\n"								\
"  -P           Write Parameters\n"								\
"  -2           List coefficients of 25-Pt Calibration\n"		\
"  -d           Dump EEPROM\n"									\

int deviceFD = -1;
EEPROM eeprom;

int optFixEEPROM = 0;
int optVersion = 0;
int optType = 0;
int optEEPROM = 0;
int opt25Pt = 0;
int optPerfParm = 0;
int optPerfParmWrite = 0;
char optPerfParmWriteDat[100];

char *RomFile;

void GetOpt(int argc, char **argv)
{   char c;

	opterr = 1;
	while ( (c=getopt(argc, argv, "avtp2dP:")) != -1 )
	{	switch (c)
		{
		case 'a':
			optVersion	= 1;
			optType		= 1;
			optPerfParm	= 1;
			opt25Pt		= 1;
			optEEPROM	= 1;
			return;			
		case 'v':
			optVersion = 1;
			break;
		case 't':
			optType = 1;
			break;
		case 'p':
			optPerfParm = 1;
			break;
		case '2':
			opt25Pt = 1;
			break;
		case 'd':
			optEEPROM = 1;
			break;
		case 'P':
			optPerfParmWrite = 1;
			strcpy(optPerfParmWriteDat, optarg);
			break;
		}
	}
}

void DumpEEPROM()
{	int i;
	printf("D\n");
	for (i=0; i<EEPROM_SIZE; i++)
	{	printf("<%02d:%04X> ", i, eeprom[i]);
		if ( !((i+1)%8) )
		{	printf("\n");
		}
	}     
}

void List25Parm()
{	int i;

	printf("2\n[%d] \n", eeprom[EPR_Flags] & EPR_F_25CoefOn);
	for (i=1; i<=25; i++)
	{	EEPROM_UNIT dat = eeprom[EPR_25(i)];
		printf("(%d,%d) ", EPR_25X(dat), EPR_25Y(dat));
		if ( !(i%5) )
		{	printf("\n");
		}
	}
	for (i=EPR_25LLX; i<=EPR_25URY; i++)
	{	printf("<%d> ", eeprom[i]);
	}
	printf("\n");
}

void ListPerfParm()
{	EEPROM_UNIT dat = eeprom[EPR_SlowFast];
	EEPROM_UNIT dat2 = eeprom[EPR_LiftDelay];
	EEPROM_UNIT flag = eeprom[EPR_Flags];

	printf("P\n<%d %d %d %d %d %d %d %d %d>\n",
			eeprom[EPR_AccLimit],
			eeprom[EPR_AD_DevLimit],
			eeprom[EPR_Debounced],
			EPR_Fast(dat), EPR_Slow(dat),
			(flag & EPR_F_FwFilterOn) != 0,
			(flag & EPR_F_PowerSaveOn) != 0,
			EPR_LiftCnts(dat2), EPR_LiftRange(dat2)
			);
}

void WritePerfParm()
{	int AccLimit, AD_DevLimit, Debounced, Fast, Slow;
	int FilterOn, PowerSaveOn, LiftCount, LiftRange;

	if ( sscanf(optPerfParmWriteDat, "_%d_%d_%d_%d_%d_%d_%d_%d_%d", 
			&AccLimit, &AD_DevLimit, &Debounced, &Fast, &Slow,
			&FilterOn, &PowerSaveOn, &LiftCount, &LiftRange
			) == 9 )
	{	eeprom[EPR_AccLimit]	= AccLimit;
		eeprom[EPR_AD_DevLimit] = AD_DevLimit;
		eeprom[EPR_Debounced]	= Debounced;
		eeprom[EPR_SlowFast]	= EPR_SlowFastMerge(Slow, Fast);
		eeprom[EPR_Flags]		&= ~(EPR_F_FwFilterOn | EPR_F_PowerSaveOn);
		eeprom[EPR_Flags]		|= (FilterOn ? EPR_F_FwFilterOn : 0);
		eeprom[EPR_Flags]		|= (PowerSaveOn ? EPR_F_PowerSaveOn : 0);
		eeprom[EPR_LiftDelay]	= EPR_LiftDelayMerge(LiftCount, LiftRange);
		FixEEPROM(eeprom);
		{	int rval = 0;
			rval |= _WriteEEPROM(deviceFD, eeprom, EPR_Flags, EPR_CheckSum);
			rval |= _WriteEEPROM(deviceFD, eeprom, EPR_LiftDelay, EPR_LiftDelay);
			printf("W\n<%d>\n", rval);
		}
		SendSimpleCommand(deviceFD, H2D_FIRMWARERESET);
	}else
	{	printf("W\nBad Format\n");
	}
}

void FixEEPROMchecksum()
{	FixEEPROM(eeprom);
	printf("F\n<%d>\n", 
			_WriteEEPROM(deviceFD, eeprom, EPR_CheckSum, EPR_CheckSum));
}

void ProcessOption()
{	if (optType)
	{	int cmd = H2D_GETTYPE, len = 0;
		char dummy[8], *ptr = dummy;
		if ( !SendAndWait(deviceFD, &cmd, &ptr, &len) && cmd == D2H_TYPE )
		{	ptr[len] = '\0';
			printf("T\n%s\n", ptr);
		}
	}
	if (optVersion)
	{	int cmd = H2D_GETVERSION, len = 0;
		char dummy[8], *ptr = dummy;
		if ( !SendAndWait(deviceFD, &cmd, &ptr, &len) && cmd == D2H_VERSION )
		{	ptr[len] = '\0';
			printf("V\n%s\n", ptr);
		}
	}
	if (optEEPROM)
	{	DumpEEPROM();
	}
	if (opt25Pt)
	{	List25Parm();
	}
	if (optPerfParm)
	{	ListPerfParm();
	}
	if (optPerfParmWrite)
	{	WritePerfParm();
	}
	if (optFixEEPROM)
	{	FixEEPROMchecksum();
	}
}

void SetRomFilename(char *name)
{	static char buf[100];
	char *slash;
	
	if ( (slash = strrchr(name, '/')) )
	{	name = slash + 1;
	}
	
	sprintf(buf, IDcat(PRJNAME, _ROMHOME) ROM_FORMAT, name);
	RomFile = buf;
}
                        

int main(int argc, char **argv)
{	int EEPROM_OK = 0;

	if (argc == 1)
	{	printf(GetPanelInfoHelp);
		return 0;
	}
	if ( (deviceFD = open(argv[1], O_RDWR)) < 0 )
	{	puts("S\nopen failed");
		return 0;
	}

	GetOpt(argc-1, argv+1);

	Dev_SaveState(deviceFD);
	Dev_SetBaud(deviceFD, 9600);
	
	StopDriver();
		
	if (!LoopBackOK(deviceFD))
	{	puts("S\nno responce");
		Dev_RestoreState(deviceFD);
		close(deviceFD);
		return 0;
	}
	
	SetRomFilename(argv[1]);
	
	if (CachedReadEEPROM(RomFile, deviceFD, eeprom))
	{	puts("S\nbad eeprom");
		// optFixEEPROM = 0; // Fix EEPROM Checksum is allowed
		optVersion = 0;
		optType = 0;
		// optEEPROM = 0;	// EEPROM dump is allowed
		opt25Pt = 0;
		optPerfParm = 0;
		optPerfParmWrite = 0;
	}else
	{	EEPROM_OK = 1;
	}
	ProcessOption();

	Dev_RestoreState(deviceFD);
	close(deviceFD);
	
	if ( EEPROM_OK ) puts("S\nOK");
	return 0;	
}

