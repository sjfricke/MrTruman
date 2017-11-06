#! /bin/sh
# the next line restarts using tclsh \
	exec tclsh "$0" "$@"


####################################
#  (Procedures Begin) {
####################################

proc extract {srcdest pattern} {
	upvar $srcdest org
	set lines [split $org "\n"]
	foreach i $lines {
		if { [string first $pattern $i] >= 0 } {
			set org $i
			return 1
		}
	}
	return 0
}

proc AddXModuleSection_4 {in out} {
	global Action
	global PRJNAME
	global BeginTag EndTag
	global XI_PANEL DRIVER_NAME ${PRJNAME}_DAT
	set state NORMAL
	set panelExist 0

	while { [gets $in line] >= 0 } {
		set upline [string toupper $line]
		switch $state {
			NORMAL {
				if { [string first $BeginTag $line] >= 0 } {
					set state PRJSECTION
					continue
				}
				if { [scan $upline "%s%s" mark name] == 2 } {
					if { ![string compare $mark "SECTION"] } {
						if { ![string compare $name "\"SERVERLAYOUT\""] } {
							set state SERVERLAYOUT
						}
					}
				}
			}
			SERVERLAYOUT {
				set first [lindex $upline 0]
				if { ! [string compare $first "ENDSECTION"] } {
					if { !$panelExist && $Action > 0 } {
						puts $out "        InputDevice    \"$XI_PANEL\" \"SendCoreEvents\""
					}
					set state NORMAL
				} else { if { ! [string compare $first "INPUTDEVICE"] } {
					set second [lindex $line 1]
					if { ! [string compare $second "$XI_PANEL"] } {
						incr panelExist
						if { $Action < 0 } continue
					}
				} }
			}
			PRJSECTION {
				if { [string first $EndTag $line] >= 0 } {
					set state NORMAL
				}
				continue
			}
		}
		puts $out $line
	}
	if { [string compare $state "NORMAL"] } {
		global XF86CONFIG
		puts "************************** Warning ****************************"
		puts "** End tag of ${PRJNAME} section not found                     "
		puts "**                                                             "
		puts "** Your $XF86CONFIG might be truncated                         "
		puts "***************************************************************"
	} else {
		if { $Action > 0 } {
			puts $out $BeginTag
			set msg \
{	Section "InputDevice"
		Identifier  "$XI_PANEL"
		Driver      "$DRIVER_NAME"
		Option      "Device"        "[subst $${PRJNAME}_DAT]0"
		Option      "DebugLevel"    "0"
	EndSection}
			puts $out [subst $msg]
			puts $out $EndTag
		}
	}
}

proc AddXModuleSection_3 {in out} {
	global PRJNAME
	global BeginTag EndTag
	global PANEL_SECTION_NAME MODULE_NAME ${PRJNAME}_DAT ${PRJNAME}_DEVICENAME

	set state NotFound
	while { [gets $in line] >= 0 } {
		switch $state {
			NotFound {
				if { [string first $BeginTag $line] >= 0 } {
					set state Found
				} else {
					puts $out $line
				}
			}
			Found {
				if { [string first $EndTag $line] >= 0 } {
					set state NotFound
				}
			}
		}
	}
	if { ![string compare $state "Found"] } {
		global XF86CONFIG
		puts "************************** Warning ****************************"
		puts "** End tag of ${PRJNAME} section not found                     "
		puts "**                                                             "
		puts "** Your $XF86CONFIG might be truncated                         "
		puts "***************************************************************"
	} else {
		global Action
		if { $Action > 0 } {
			puts $out "$BeginTag"
			set record \
{	Section "Module"
		Load "xf86${MODULE_NAME}.so"
	EndSection
	Section "Xinput"
		SubSection "$PANEL_SECTION_NAME"
			Port "[subst $${PRJNAME}_DAT]0"
			DeviceName "[subst $${PRJNAME}_DEVICENAME]"
			AlwaysCore
			DebugLevel 0
		EndSubSection
	EndSection}
			puts $out [subst $record]
			puts $out $EndTag
		}
	}
}


####################################
#  (Procedures End) }
####################################

source utility/library.tcl

if { $argc } {
	#uninstall
	set Action -1
} else {
	#install
	set Action 1
}

set tool_path "include/"

if {[LoadGlobalSetting ${tool_path}]} {
	puts "Can not load global settings"
	exit 1
}

set xversion [exec tcl utility/xversion.tcl]

if { 410 > 400 } {
	set XF86Config XF86Config-4
#	set XF86Config XF86Config
	set AddXModuleSection AddXModuleSection_4
} else {
	set XF86Config XF86Config
	set AddXModuleSection AddXModuleSection_3
}

set BeginTag "## ${PRJNAME} section begin (Please do NOT edit this section!!) ##"
set EndTag "## ${PRJNAME} section end ##"

set rval [catch {exec find /etc -name $XF86Config -print} XF86CONFIG]

if { [extract XF86CONFIG $XF86Config] } {
	if [catch {open $XF86CONFIG r+} result] {
		puts "************************** Warning ****************************"
		puts "** $result"
		puts "***************************************************************"
	} else {
		puts "(*) Update XFree86 configuration \[$XF86CONFIG]"
		set filein $result
		if { [catch {open $XF86CONFIG.tmp w} fileout] } {
			close $filein
			puts "************************** Warning ****************************"
			puts "** Can not open tmp file to update $XF86CONFIG                 "
			puts "**                                                             "
			puts "** Please modify your XFree86 setting manually                 "
			puts "** (see README.X for detail)                                   "
			puts "***************************************************************"
		} else {
			global AddXModuleSection
			$AddXModuleSection $filein $fileout
			close $filein
			close $fileout
			catch {file rename -force $XF86CONFIG $XF86CONFIG.bak}
			catch {file rename -force $XF86CONFIG.tmp $XF86CONFIG}
		}
	}
} else {
	puts "************************** Warning ****************************"
	puts "** $XF86Config: File not found                                 "
	puts "***************************************************************"
}

