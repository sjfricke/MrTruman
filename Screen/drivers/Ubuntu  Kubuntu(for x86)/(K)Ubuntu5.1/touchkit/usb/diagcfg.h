
#ifndef _DIAGCFG_H_
#define _DIAGCFG_H_

#include "configINT.h"

#define IDLETIMEOUT				15
#define BLINKPERD				0.16        

#define ROUND_SYMBOL

#if 0
	#define Background				cBLACK
	#define	TouchedCross			cYELLOW
	#define BlinkCrossBG			cRED
	#define BlinkCrossFG			cWHITE
	#define nTouchedCross			cBLUE
	#define DrawCross				cYELLOW
	#define DrawGrid				cGREEN
	#define DrawLine				cYELLOW
	#define DrawUp					cRED
	#define DrawDown				cGREEN
	#define TimerLine				cRED
	#define PromptText				cWHITE
#else
	#define Background				cCYAN
	#define	TouchedCross			cYELLOW
	#define BlinkCrossBG			cRED
	#define BlinkCrossFG			cWHITE
	#define nTouchedCross			cBLUE
	#define Cross					cWHITE
	#define DrawGrid				cWHITE
	#define DrawLine				cYELLOW
	#define DrawUp					cRED
	#define DrawDown				cBLUE
	#define TimerLine				cRED
	#define PromptText				cBLUE
#endif

#define         N_Colors    10

static char     colors[N_Colors][10] =
				{	"BLACK", "WHITE", "RED", "YELLOW", "GREEN", "BLUE",
					"#40C0C0"
				};
				
static unsigned long   pixels[N_Colors];

#define cBLACK					(pixels[0])
#define cWHITE					(pixels[1])
#define cRED					(pixels[2])
#define cYELLOW					(pixels[3])
#define cGREEN					(pixels[4])
#define cBLUE					(pixels[5])
#define cCYAN					(pixels[6])

#define _25PCAL_HELP								\
	"\n"											\
	"== 25-Points Calibration ==\n"					\
	"\n"											\
	"Syntax: %s <device>\n"							\
	"\n"

#define _4PCAL_HELP									\
	"\n"											\
	"== 4-Points Calibration ==\n"					\
	"\n"											\
	"Syntax: %s <device>\n"							\
	"\n"

#define DRAW_HELP									\
	"\n"											\
	"== Draw Test ==\n"								\
	"\n"											\
	"Syntax: %s <device> [raw]\n"					\
	"\n"

#define PRINT_HELP(fmt, name)		__PRINT_HELP((fmt), name)
#define __PRINT_HELP(fmt, name)		(printf((fmt), #name))
#define PRINT_25PCAL_HELP			PRINT_HELP(_25PCAL_HELP, _25PCAL_NAME)
#define PRINT_4PCAL_HELP			PRINT_HELP(_4PCAL_HELP, _4PCAL_NAME)
#define PRINT_DRAW_HELP				PRINT_HELP(DRAW_HELP, DRAW_NAME)

#endif

