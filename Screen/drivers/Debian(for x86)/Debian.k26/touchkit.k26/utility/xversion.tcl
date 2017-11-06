#!/bin/sh
#\
exec tclsh "$0" "$@"

catch {exec X -version} msg
set idx [lsearch -regexp $msg {[Vv]ersion}]
if { $idx < 0 } {
	puts 0
} else {
	incr idx
	set ver [lindex $msg $idx]
	set ver [split $ver "."]
	set num 0
	foreach piece $ver {
		set digit 0
		scan $piece "%d" digit
		set num [expr 10 * $num + $digit]
	}
	puts $num
}

