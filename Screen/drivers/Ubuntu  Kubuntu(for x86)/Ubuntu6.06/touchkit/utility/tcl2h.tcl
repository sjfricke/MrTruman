#!/bin/sh
# the next line restarts using tclsh \
	exec tclsh "$0" "$@"
	
if {$argc} {
	set filename [lindex $argv 0]
} else {
	return 1
}

proc ConvertSTR {filename} {
	if { [catch {source $filename} rval] } {
		puts stderr $rval
		exit 1
	}
	unset rval
	unset filename
	set vars [info locals]
	foreach name "$vars" {
		set cmd "set val \"\\\"$$name\\\"\""; eval $cmd
		puts "#ifndef $name"
		puts [format "#define %20s   %s" $name $val]
		puts "#endif"
	}
}

proc ConvertINT {filename} {
	if { [catch {source $filename}]} {
		return 1
	}
	unset filename
	set vars [info locals]
	foreach name "$vars" {
		set cmd "set val $$name"; eval $cmd
		puts "#ifndef $name"
		puts [format "#define %20s   %s" $name $val]
		puts "#endif"
	}
}

set name [split $filename "."]
set name [lindex $name 0]
set name [string toupper $name]
puts "#ifndef _${name}_H_"
puts "#define _${name}_H_\n"

if {[string first "INT" $name] >= 0} {
	ConvertINT $filename
} else {
	ConvertSTR $filename
}

puts "\n#endif\n"

