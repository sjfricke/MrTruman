#!/bin/sh
# the next line restarts using tclsh \
	exec tclsh "$0" "$@"

#TAB SIZE: 4

set PRJNAME				TouchKit
set XDRIVER_NAME		touchkit

set SM_NONE				0
set SM_DOWN				1
set SM_UP				2
set DefaultSoundMode	$SM_NONE

set TM_DRAWING			0
set TM_DESKTOP			1
set DefaultMouseMode	$TM_DRAWING

set DefaultDblClickSpeed	18
set DefaultDblClickArea		30
set DefaultRClickTool		0

set MaxDblClickSpeed	8
set MinDblClickSpeed	28
set MaxDblClickArea		50
set MinDblClickArea		15
set MaxRClickTool		1
set MinRClickTool		0

set FrameSize			5

set ${PRJNAME}_MODE		[expr 0644]

binary scan "e" c magic1
binary scan "G" c magic2

set AP2D_CMD_BASE		[expr $magic1 * $magic2]

set AP2D_SHOWSTATUS		[incr AP2D_CMD_BASE]
set AP2D_SHAREPORT		[incr AP2D_CMD_BASE]
set AP2D_CLOSEPORT		[incr AP2D_CMD_BASE]
set AP2D_SENDDATA		[incr AP2D_CMD_BASE]
set AP2D_SETDEBUG		[incr AP2D_CMD_BASE]
set AP2D_RCLICK			[incr AP2D_CMD_BASE]
set AP2D_RESET			[incr AP2D_CMD_BASE]
set AP2D_SUICIDE		[incr AP2D_CMD_BASE]
set AP2D_SETMODE		[incr AP2D_CMD_BASE]
set AP2D_SETSOUNDMODE	[incr AP2D_CMD_BASE]
set AP2D_SETDCSPEED		[incr AP2D_CMD_BASE]
set AP2D_SETDCAREA		[incr AP2D_CMD_BASE]
set AP2D_RCLICKON		[incr AP2D_CMD_BASE]
set AP2D_RCLICKOFF		[incr AP2D_CMD_BASE]
set AP2D_RCLICKDELAYON	[incr AP2D_CMD_BASE]
set AP2D_FLUSHOUTPUT	[incr AP2D_CMD_BASE]
set AP2D_RCLICKCANCEL	[incr AP2D_CMD_BASE]
set AP2D_SETEEPROM		[incr AP2D_CMD_BASE]
set AP2D_XMODULEREADY	[incr AP2D_CMD_BASE]
set AP2D_GETMODE		[incr AP2D_CMD_BASE]

set RCLICK_DELAY		2

set FIFO_MODE			[expr 0644]

unset magic1 magic2 

set _LeftButton			1
set _MidButton			2
set _RightButton		3

set TPANELD_NAME		tpaneld
set TOUCHCFG_NAME		touchcfg
set _25PCAL_NAME		25pcal
set _4PCAL_NAME			4pcal
set DRAW_NAME			drawtest

set USBPNPD_NAME		usbpnpd
set USBMODULE_NAME		tkusb

set ${PRJNAME}_USB_VID	[expr 0x3823]
set ${PRJNAME}_USB_PID	[expr 0x0001]

set SCR_MAX_MNR			2
set SCR_BASE_MNR		180

set EPR_DefaultAccLimit		256
set EPR_DefaultADLimit		50
set EPR_DefaultDebounced	7
set EPR_DefaultFast			36
set EPR_DefaultSlow			10

set EPR_DefaultFilterOn		0
set EPR_DefaultPowerSaveOn	0
set EPR_DefaultLiftCount	0
set EPR_DefaultLiftRange	20

#These two would be overriden by [User] section of touch.ini
set V_btAdvancedOn			1
set PerfTuningOn			0

