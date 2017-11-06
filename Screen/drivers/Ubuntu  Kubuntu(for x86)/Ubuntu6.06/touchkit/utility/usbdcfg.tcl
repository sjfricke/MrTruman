#!/bin/sh
# the next line restarts using tclsh \
	exec tclsh "$0" "$@"

proc FixIDlist {victim} {
	set result ""
	foreach item $victim {
		set item [string range $item 0 3]
		lappend result [format "%04s" $item]
	}
	return $result
}

####################################
#  main()
####################################

source utility/library.tcl

set tool_path "include/"

if {[LoadGlobalSetting ${tool_path}]} {
	puts "Can not load global settings"
	exit 1
}

if { [catch {open $PNP_CONFIG w} fd] } {
	puts $fd
	exit 1
}

if { [info exists USB_VID_MORE] && [info exists USB_PID_MORE] } {
	set USB_VID_MORE [FixIDlist $USB_VID_MORE]
	set USB_PID_MORE [FixIDlist $USB_PID_MORE]
	
	set USBPNPD_ACTION												\
		    [subst {insmod $USBMODULE_NAME \
    		vidlist=\"$USB_VID_MORE\" pidlist=\"$USB_PID_MORE\"; \
    		/usr/bin/$TPANELD_NAME &}]
    
	puts $fd "#Device = VendorID, ProductID, ActionIfPlugIn\n"
	
	foreach vid $USB_VID_MORE pid $USB_PID_MORE {
		puts $fd "Device = 0x$vid, 0x$pid, $USBPNPD_ACTION"
	}       
} else {
	puts $fd $DefaultUSBpnpdConf
}

close $fd
