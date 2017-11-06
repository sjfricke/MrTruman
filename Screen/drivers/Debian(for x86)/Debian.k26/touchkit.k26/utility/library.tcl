#! /bin/sh
# the next line restarts using tclsh \
	exec tclsh "$0" "$@"

proc {LoadGlobalSetting} {path} {
	set filelist "configINT.tcl configSTR.tcl touch.tcl"
	
	foreach filename $filelist {
		if { [catch {source $path/$filename} rval]} {
			puts $rval
		    return 1
		}
	}
	unset filename
	set vars [info locals]
	foreach name "$vars" {
		if { [string length $name] } {
		    set cmd "set val \"$$name\""; eval $cmd
		    unset $name
		    global $name
		    set $name $val
		}
	}
	return 0
}

proc IsComment {line} {
	set first [lindex $line 0]
	if { [string first "#" $first] == 0 } {
		return 1
	} else {
		return 0
	}
}

