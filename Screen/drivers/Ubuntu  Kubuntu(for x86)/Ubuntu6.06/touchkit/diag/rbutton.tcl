#!/bin/sh
# \
exec wish "$0" "$@"
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

set DefaultDeviceList	"/dev/ttyS0 /dev/ttyS1 /dev/psaux /dev/tkpanel0 /dev/tkpanel1"

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




#[User]
set             User_Advanced "True"
set           User_Adjustment "False"

#[About]
set               About_Title "About"
set             About_LogoGif "logo.gif"
set           About_Copyright "copyright.txt"
set             About_Profile "info.txt"
set                  About_OK "OK"

#[Main]
set                Main_Title "TouchCfg"
set              Main_LogoGif "logo.gif"
set            Main_Interface "Interface"
set                 Main_Type "Type"
set            Main_FWVersion "Version"
set               Main_Status "Status"
set       Main_PanelCandidate "Panel Candidate List"
set             Main_AddPanel "Add"
set          Main_RescanPanel "Rescan"
set          Main_RemovePanel "Remove"
set                   Main_OK "OK"
set              Main_Testing "Testing..."
set         Main_E_noresponce "E (no responce)"
set           Main_E_notfound "E (not found)"
set            Main_E_notchar "E (not char device)"
set         Main_E_openfailed "E (open failed)"
set            Main_E_unknown "E (?)"
set                Main_Sound "Sound"
set              Main_NoSound "No Sound"
set       Main_SoundTouchDown "Touch Down"
set         Main_SoundTouchUp "Touch Up"
set            Main_MouseMode "Mouse Mode"
set              Main_Desktop "Desktop Mode"
set              Main_Drawing "Drawing Mode"
set           Main_ModeButton "Mode Button"
set     Main_DoubleClickSpeed "Dbl-Click Speed:"
set      Main_DoubleClickArea "Dbl-Click Area:"
set             Main_SlowDown "<< Slower"
set              Main_SpeedUp "Faster >>"
set           Main_ShrinkArea "<< Smaller"
set           Main_ExpandArea "Larger >>"
set             Main_DrawTest "Draw Test"
set               Main_Cal4Pt "4-Pt Cal"
set                Main_About "About"
set                 Main_Quit "Close"
set             Main_Advanced "Advanced >>"

#[AddPanel]
set            AddPanel_Title "Add Panel"
set        AddPanel_Interface "Interface:"
set               AddPanel_OK "OK"
set           AddPanel_Cancel "Cancel"
set       AddPanel_E_notfound "<file not found>"
set        AddPanel_E_notchar "<not char device>"
set          AddPanel_E_exist "<already in list>"

#[Advanced]
set            Advanced_Title "Advanced Settings"
set          Advanced_Cal25Pt "25-Pt Coef"
set        Advanced_Calibrate "Calibrate"
set            Advanced_Clear "Clear"
set         Advanced_PerfTune "Perf Tuning"
set       Advanced_ParameterA "Parameter A"
set       Advanced_ParameterB "Parameter B"
set       Advanced_ParameterC "Parameter C"
set       Advanced_ParameterD "Parameter D"
set       Advanced_ParameterE "Parameter E"
set          Advanced_Default "Default"
set            Advanced_Apply "Apply"
set            Advanced_Close "Close"

#[Exec]
set                Exec_Title "Exec Command"
set                   Exec_OK "OK"

#[ModeButton]
set          ModeButton_Mode0 "Drawing"
set          ModeButton_Mode1 "Desktop"


#!/bin/sh
# the next line restarts using wish\
	exec wish "$0" "$@" 

set RCLICK_TIMEOUT	30000
set AFTER_CMD		""
set BUTTON_COLOR	blue

set OutputChannel /dev/pts/10
set OutputChannel /dev/null

###########################################################################

proc {Output} {msg} {
	global OutputChannel
	catch {exec echo $msg > $OutputChannel}
}

proc PackLRButton {widget} {
	pack $widget -side left -expand 1 -fill both
}	

proc NewLRButton {widget} {
	button $widget -padx 0 -pady 0
}

proc CreateImage {name} {
	if {[string length $name]} {
		if { ![catch {image create photo -file $name} img] } {
			return $img
		}
	}
	return 0
}

proc SetFont {orgFont} {
	set extra ""
	if { [scan $orgFont "%s -%d %s" name size extra] < 2 } {
		return $orgFont
	}
	set sWidth [winfo screenwidth .]
	set size [expr int($sWidth*$size/1024)]
	
	if { $size < 10 } {
		set extra ""
	}
	if { $size < 8 } {
		set size 8
	}
	return "$name -$size $extra"
}

proc SetButton {bname imgsrc txtsrc anchor} {
	global wWidth wHeight
	set rval [CreateImage $imgsrc]
	if {[string compare $rval 0]} {
		if { [image width $rval] > $wWidth } {
			set wWidth [image width $rval]
		} 
		if { [image height $rval] > $wHeight } {
			set wHeight [image height $rval]
		}
		$bname configure -image $rval
	} else {
		set font [$bname cget -font]
		set font [SetFont $font]
		$bname configure -text $txtsrc -anchor $anchor -font $font
	}
}

proc EnterRightMode {} {
	pack forget .frMain.frL .frMain.frR
	pack .frMain.frR -expand 1 -fill both
}

proc EnterLeftMode {} {
	pack forget .frMain.frL .frMain.frR
	pack .frMain.frL -expand 1 -fill both
}

proc ReportModeToTouchcfg {mode} {
	global TOUCHCFG_CMD
	if { [file exists $TOUCHCFG_CMD] 
		&& ! [string compare [file type $TOUCHCFG_CMD] "fifo"] } {
		if { ![catch {open $TOUCHCFG_CMD r+} file] } {
			puts $file $mode
			close $file
		} else {
			Output $file
		}
	}
}

proc Pressed {which} {
	global AP2D_SETMODE AP2D_RCLICK ETTOUCH_CMDIN
	switch $which {
	L {
			global PRJNAME
			global ${PRJNAME}_CMDIN
			global RCLICK_TIMEOUT AFTER_CMD
			catch {exec echo \[$AP2D_RCLICK\][pid] > [subst $${PRJNAME}_CMDIN]}
			EnterRightMode
			set AFTER_CMD [
				after $RCLICK_TIMEOUT {
					global AP2D_RCLICKCANCEL
					catch {exec echo \[$AP2D_RCLICKCANCEL\][pid] > [subst $${PRJNAME}_CMDIN]}
					set AFTER_CMD ""
					EnterLeftMode
				}
			]
		}
	MODE {
		global PRJNAME
		global ${PRJNAME}_CMDIN
		set idx [Text2ModeIdx [.frMain.btMode cget -text]]
		set idx [GetNextIdx $idx]
		catch {exec echo \[$AP2D_SETMODE\]$idx > [subst $${PRJNAME}_CMDIN]}
		ReportModeToTouchcfg [ModeIdx2Text $idx]
		.frMain.btMode configure -text [ModeIdx2Text $idx] 
	}
	}
}

proc RecvCmd {} {
	if { [gets stdin line] < 0 } {
		exit
	}
	global RCLICK_LOCK
	
	Output "RecvCmd <$line>"
	
	switch -regexp $line {
		OK {
			global PRJNAME
			global AFTER_CMD
			global ${PRJNAME}_CMDIN
			catch {after cancel $AFTER_CMD}
			set AFTER_CMD ""
			EnterLeftMode
		}
		ON {
			wm deiconify .
			catch {exec echo [pid] 1 > $RCLICK_LOCK}
		}
		OFF {
			wm withdraw .
			catch {exec echo [pid] 0 > $RCLICK_LOCK}
		}
		BEEP {
			bell	
			Output "Beep Request"
			}
		. {
			if { [scan $line "%d" num] } {
				.frMain.btMode configure -text [ModeIdx2Text $num] 
			 }
		}
	}
}		

proc CheckParent {ppid} {
	global argv0 argv
	if { [catch {exec ps j} val] } {
		Output "exec ps: $val"
	}
	set pid [pid]
	set group [split $val "\n"]
	set sample [string toupper [lindex $group 0]]
	set pid [pid]
	set pididx [lsearch $sample PID]
	set ppididx [lsearch $sample PPID]
	foreach line $group {
		if { [lindex $line $pididx] == $pid } {
			if { $ppid == [lindex $line $ppididx] } {
				return 0	
			}
		}
	}
	return 1
}

proc FixText {text} {
	set head [string index $text 0]
	set tail [string range $text 1 end]
	return "[string toupper $head][string tolower $tail]"
}

proc _ModeListInit {} {
	global _ModeList
	set _ModeList ""
	for {set i 0} {1} {incr i} {
		global ModeButton_Mode$i
		if { [info exists ModeButton_Mode$i] } {
			lappend _ModeList [subst \$ModeButton_Mode$i]
		} else {
			break
		}
	}
}

proc ModeListInit {} {
	global _ModeListInited
	if { ![info exists _ModeListInited] } {
		_ModeListInit
		set _ModeListInited 1
	}	
}

proc ModeIdx2Text {idx} {
	global _ModeList
	ModeListInit
	return [lindex $_ModeList $idx]
}	

proc Text2ModeIdx {text} {
	global _ModeList
	ModeListInit
	return [lsearch $_ModeList $text]
}

proc GetNextIdx {idx} {
	global _ModeList
	ModeListInit
	set cnt [llength $_ModeList]
	incr idx
	if { $idx >= $cnt } {
		set idx 0
	}
	return $idx
}


#################################################################
set lmsg L
set mmsg M
set rmsg R

set lanchor w
set manchor center
set ranchor e

if { [catch {winfo screenheight .}] } {
	exit
}

set wHeight [expr int([winfo screenheight .] * 0.08)]
set wWidth $wHeight

set Pad [expr int($wHeight * 0.625)]

#################################################################

if { [CheckParent [lindex $argv 2]] } {
	Output "Not called by $TPANELD_NAME"
	exit
}

set Status [lindex $argv 3]
catch {exec echo [pid] $Status > $RCLICK_LOCK}
catch {exec chmod 666 $RCLICK_LOCK}

fileevent stdin readable RecvCmd

# We don't need SetButton(*.gif, alterText, anchor) now
#
# SetButton .frR.btL [lindex $argv 0]	$lmsg $lanchor 

frame .frMain -relief groove -borderwidth 2
pack .frMain -expand 1 -fill both

button .frMain.btMode -text [ModeIdx2Text [lindex $argv 4]] \
	-padx 0 -pady 0 -command {Pressed MODE}
.frMain.btMode configure -font [SetFont [.frMain.btMode cget -font]]
pack .frMain.btMode -side bottom -expand 1 -fill both


foreach fr {L R} {
	set frame .frMain.fr$fr
	frame $frame
	foreach bt {L R} {
		NewLRButton $frame.bt$bt
		PackLRButton $frame.bt$bt
	}
}

.frMain.frL.btL configure -command {Pressed L} \
	-background $BUTTON_COLOR -activebackground $BUTTON_COLOR
.frMain.frL.btR configure -command {Pressed L}

.frMain.frR.btR configure \
	-background $BUTTON_COLOR -activebackground $BUTTON_COLOR
.frMain.frR.btL configure


set joblist ""
bind . <Visibility> {
	catch {exec ps -A} psx
	if { [string first $_4PCAL_NAME $psx] >= 0 } {
	} else { if { [string first $DRAW_NAME $psx] >= 0 } {
	} else { if { [string first $_25PCAL_NAME $psx] >= 0 } {
	} else {
		lappend joblist [
			after 200 {
				if { [llength $joblist] } {
					raise . 
					set joblist ""
				}
			}
		]
		update
	} } }
}
EnterLeftMode

. configure -cursor left_ptr
wm geometry . [expr $wWidth+4]x[expr $wHeight+4]-$Pad-$Pad
wm withdraw .
wm overrideredirect . 1
wm deiconify .

if { ! $Status } {
	after 200 {wm withdraw .}
}



