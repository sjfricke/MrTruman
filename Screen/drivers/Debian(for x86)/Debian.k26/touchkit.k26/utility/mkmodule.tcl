#!/bin/sh
# the next line restarts using tclsh \
	exec tclsh "$0" "$@"
	
if { $argc < 3 } {exit 1}

set filename [lindex $argv 0]
set from [lindex $argv 1]
set fromlen [string length $from]
set to [lindex $argv 2]
set to [lindex [split $to "."] 0]
set tail [lrange $argv 3 end]

set inserted 0

proc replace_in_line {victim} {
	global from fromlen to tail
	set line ""

	while { [string length $victim] } {
		set locate [string first $from $victim]
		if { $locate >= 0 } {
			append line [string range $victim 0 [incr locate -1]]
			append line $to
			set victim [string range $victim [expr $locate + 1 + $fromlen] end]
		} else {
			append line $victim
			break
		}
	}
	global inserted
	if { ! $inserted } {
		set cmd "set line \[linsert \$line 1 $tail]"
		set inserted 1
	} else {
		set cmd ""
	}
	catch {eval $cmd}
	if {$inserted == 1} {incr inserted; puts $line}
	set cmd "exec $line"
	catch {eval $cmd} r
	puts $r	
}

if { [catch {open $filename "r"} fp] } {
	exit 1
}
while { [gets $fp buf] >= 0 } {
	replace_in_line $buf	
}

return 0
