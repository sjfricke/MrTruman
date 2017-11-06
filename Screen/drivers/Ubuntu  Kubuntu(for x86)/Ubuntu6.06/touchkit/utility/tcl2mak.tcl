#!/bin/sh
# the next line restarts using tclsh \
	exec tclsh "$0" "$@"
	
if {$argc} {
	set filename [lindex $argv 0]
} else {
	return 1
}

proc GetSetting {filename} {
	if { [catch {source $filename} rval] } {
		puts stderr $rval
		exit 1		
	}
	unset rval
	unset filename
	set vars [info locals]
	foreach name "$vars" {
		set cmd "set val $$name"; eval $cmd
		puts [format "%20s = %s" $name $val]
	}
}

puts "\n"
GetSetting $filename
puts "\n"


