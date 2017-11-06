#!/bin/sh
#\
exec tclsh "$0" "$@"

if { $argc < 2 } exit

set from [lindex $argv 0]
set fromlen [string length $from]
set to [lindex $argv 1]

proc replace_in_line {victim} {
	global from fromlen to

	while { [string length $victim] } {
		set locate [string first $from $victim]
		if { $locate >= 0 } {
			puts -nonewline [string range $victim 0 [incr locate -1]]
			puts -nonewline $to
			set victim [string range $victim [expr $locate + 1 + $fromlen] end]
		} else {
			puts -nonewline $victim
			break
		}
	}
	puts ""
}

while { [gets stdin buf] >= 0 } {
	replace_in_line $buf	
}

