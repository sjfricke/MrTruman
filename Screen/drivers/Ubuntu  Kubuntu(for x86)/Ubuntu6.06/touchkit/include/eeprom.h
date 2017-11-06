
#ifndef _EEPROM_H_
#define _EEPROM_H_

#define EEPROM_SIZE			64

#define EPR_LLX				0
#define EPR_LLY				1
#define EPR_LRX				2
#define EPR_LRY				3
#define EPR_ULX				4
#define EPR_ULY				5
#define EPR_URX				6
#define EPR_URY				7

#define EPR_25(idx)			(EPR_URY+(idx))

#define EPR_25X(dat)		((signed char)((dat)&0x00FF))
#define EPR_25Y(dat)		((signed char)(((dat)&0xFF00) >> 8))
#define EPR_25MERGE(x,y)	((char)(x)|((char)(y)<<8))

#define EPR_25LLX			(EPR_25(25)+1)
#define EPR_25LLY			(EPR_25(25)+2)
#define EPR_25LRX			(EPR_25(25)+3)
#define EPR_25LRY			(EPR_25(25)+4)
#define EPR_25ULX			(EPR_25(25)+5)
#define EPR_25ULY			(EPR_25(25)+6)
#define EPR_25URX			(EPR_25(25)+7)
#define EPR_25URY			(EPR_25(25)+8)

#define EPR_LiftDelay		(EPR_25URY+1)
#define EPR_Flags			(EEPROM_SIZE-6)
#define EPR_AccLimit		(EEPROM_SIZE-5)
#define EPR_AD_DevLimit		(EEPROM_SIZE-4)
#define EPR_Debounced		(EEPROM_SIZE-3)
#define EPR_SlowFast		(EEPROM_SIZE-2)
#define EPR_CheckSum		(EEPROM_SIZE-1)

/* detail of EPR_Flags */
#define EPR_F_25CoefOn		0x0001
#define EPR_F_RotationOn	0x0002
#define EPR_F_FwFilterOn	0x0004
#define EPR_F_PowerSaveOn	0x0008

#define LowByte(dat)		((unsigned char)((dat)&0x00FF))
#define HighByte(dat)		LowByte((dat)>>8)
#define ByteMerge(hi, lo)	((EEPROM_UNIT)(((int)(hi)<<8)|(char)(lo)))

#define EPR_Slow(dat)		LowByte(dat)
#define EPR_Fast(dat)		HighByte(dat)
#define EPR_SlowFastMerge(s,f)	ByteMerge(f, s)

#define EPR_LiftCnts(dat)		HighByte(dat)
#define EPR_LiftRange(dat)		LowByte(dat)
#define EPR_LiftDelayMerge(c, r)	ByteMerge(c, r)

typedef unsigned short EEPROM_UNIT;
typedef EEPROM_UNIT EEPROM[EEPROM_SIZE];

void FixEEPROM(EEPROM);
int CheckEEPROM(EEPROM);
int ReadEEPROM(int, EEPROM);
#define WriteEEPROM(fd, eeprom)		_WriteEEPROM((fd),(eeprom),0,EEPROM_SIZE-1)
int _WriteEEPROM(int, EEPROM, int, int);
int LoadEEPROM(char *filename, EEPROM);
int SaveEEPROM(char *filename, EEPROM);
void CopyEEPROM(EEPROM dest, EEPROM src);
int CachedReadEEPROM(char *romfile, int fd, EEPROM);

#endif

