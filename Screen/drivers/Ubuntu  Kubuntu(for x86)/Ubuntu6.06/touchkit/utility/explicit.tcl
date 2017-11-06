#!/bin/sh
#\
exec tclsh "$0" "$@"

if { $argc < 1 } {exit 1}

set rval 0
set filename [lindex $argv 0]
if { [catch {open $filename} file] } exit

while { [gets $file buf] >= 0 } {
	if { [string first "make\[" $buf ] } {
		puts $buf
		if { [catch {exec /bin/sh -c $buf} result] } {
			puts $result
			set rval 1
			break
		}
	}
}
close $file
exit $rval

