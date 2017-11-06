#!/bin/sh
# the next line restarts using tclsh \
	exec tclsh "$0" "$@"

#TAB SIZE: 4

catch {source configINT.tcl}

set USB_VID_MORE			"0123 3823 3823 0EEF 0EEF"
set USB_PID_MORE			"0001 0001 0002 0001 0002"

##########################################################
#				PATH 
##########################################################

set PATH_DEV				"/dev/"	
set PATH_VAR				"/var/run/"

set ${PRJNAME}_LOCK			"${PATH_VAR}tpaneld.pid"
set ${PRJNAME}_DAT			"${PATH_DEV}tkdat"
set ${PRJNAME}_CMD			"${PATH_DEV}tkcmd"
set ${PRJNAME}_CMDIN		"[subst $${PRJNAME}_CMD]in"
set ${PRJNAME}_CMDOUT		"[subst $${PRJNAME}_CMD]out"
set ${PRJNAME}_SHR			"${PATH_DEV}tkshr"
set ${PRJNAME}_SHRIN		"[subst $${PRJNAME}_SHR]in"
set ${PRJNAME}_SHROUT		"[subst $${PRJNAME}_SHR]out"

set ROM_FORMAT				"%s.rom"
set ${PRJNAME}_ROMHOME		"/etc/tpanel.rom/"
set ${PRJNAME}_CONF			"/etc/tpaneld.conf"

set TOUCHCFG_CMD			"/tmp/touchcfg.cmd"

set ${PRJNAME}_HOME			"/usr/local/${PRJNAME}/"

set XMODULE_HOME			"/usr/X11R6/lib/modules/"
set XDRV_HOME				"${XMODULE_HOME}input/"

set RCLICK_LOCK				"/tmp/rclick.lock"

set ${PRJNAME}_USBMODULE	"$PRJNAME"

set USBPORT					"/dev/tkpanel"

set DEV_LIST				"/proc/bus/usb/devices"
set PNP_CONFIG				"/etc/usbpnpd.conf"
set PRG_LOCK				"/var/run/usbpnpd.pid"

unset PATH_VAR
unset PATH_DEV

#######################################################
#					DRIVER INFO
#######################################################

set DRIVER_NAME				$XDRIVER_NAME
set MODULE_NAME				TouchKit

set XI_PANEL				"TKPANEL"
set XI_CONTROL				"PCTRL"

set PANEL_SECTION_NAME		"${DRIVER_NAME}panel"

set ${PRJNAME}_DEVICENAME	"tpanel"

#######################################################
#					DIAG STYLE
#######################################################

set AP_BACKGROUND			"black"
set AP_GRID					"green"
set AP_CROSS_OK				"yellow"
set AP_CROSS_WAIT			"blue"
set AP_TOUCH_UP				"red"
set AP_TOUCH_DOWN			"green"
set AP_LINE					"yellow"

set AP_CURSOR				"gumby"

set ModeList				"DRAWING DESKTOP"
set sDefaultMode			[lindex $ModeList 0]
foreach m $ModeList {
	set sTM_$m $m
}
unset m

set sSM_NONE				"None"
set sSM_DOWN				"Down"
set sSM_UP					"Up"

set SoundModeList			"$sSM_NONE $sSM_DOWN $sSM_UP"
set sDefaultSoundMode		"$SM_NONE"

set _Sound					"Sound"
set _Port					"Port"
set _MouseMode				"MouseMode"
set _DblClickSpeed			"DblClickSpeed"
set _DblClickArea			"DblClickArea"
set _RClickTool				"RClickTool"

set KeywordList				"$_MouseMode $_DblClickSpeed $_DblClickArea $_RClickTool $_Port $_Sound"

set DefaultDeviceList	"/dev/ttyS0 /dev/ttyS1 /dev/ttyS2 /dev/ttyS3 /dev/psaux"

if { [file exists /proc/bus/usb] } {
	if { [catch {exec cat /tmp/.usb.on} usb] } {
		#set usb ON, if open failed
		set usb 1
	}
	if { ![string compare $usb 1] } {
		for {set i 0} {$i < $SCR_MAX_MNR} {incr i} {
			lappend DefaultDeviceList ${USBPORT}$i
		}
		unset i
	}
	unset usb
}

set DefaultConfData			"\\
 $_MouseMode = $sDefaultMode \\n\\r\\
 $_DblClickSpeed = $DefaultDblClickSpeed\\n\\r\\
 $_DblClickArea = $DefaultDblClickArea\\n\\r\\
 $_RClickTool = $DefaultRClickTool\\n\\r\\
 $_Sound = $sDefaultSoundMode\\n\\r\\
"

set USBPNPD_ACTION		\
	[subst {insmod $USBMODULE_NAME; /usr/bin/$TPANELD_NAME reset}]

set DefaultUSBpnpdConf		"\\
#Device = VendorID, ProductID, ActionIfPlugIn\\
Device = [ format {0x%04X, 0x%04X, %s} \
	[subst $${PRJNAME}_USB_VID] [subst $${PRJNAME}_USB_PID] $USBPNPD_ACTION]\\n\\r\\
"

foreach device $DefaultDeviceList {
	append DefaultConfData " $_Port = $device\\n\\r\\
"
}

unset device

