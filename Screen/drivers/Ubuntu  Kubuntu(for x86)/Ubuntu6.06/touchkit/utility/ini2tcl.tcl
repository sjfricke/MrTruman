#!/bin/sh
# the next line restarts using tclsh \
	exec tclsh "$0" "$@"

proc IsSpace {char} {
	if { [string first $char " \t"] >= 0 } {
		return 1	
	} else {
		return 0
	}
}

proc ExtractWords {string} {
	set strlen [string length $string]
	set begin 0
	for {set i 0} {$i < $strlen} {incr i} {	
		if { ![IsSpace [string index $string $i]] } {
			set begin $i
			break
		}
	}
	set end [incr strlen -1]
	for {set i $end} {0 <= $i} {incr i -1} {
		if { ![IsSpace [string index $string $i]] } {
			set end $i
			break
		}
	}
	return [string range $string $begin $end]
}

proc IsComment {line} {
	set strlen [string length $line]
	for {set i 0} {$i < $strlen} {incr i} {
		set char [string index $line $i]
		if { ![IsSpace $char] } {
			if { ![string compare $char "#"] } {
				return 1			
			} else {
				break
			}
		}
	}
	return 0
}

proc IsMode {line} {
	return [regexp {\[(.*)\]} $line dummy]
}

proc ExtractMode {line} {
	if { [regexp {\[(.*)\]} $line dummy mode] } {
		set list ""
		foreach word $mode {
			append list "$word"
			append list "_"
		}
		return $list		
	}
}

proc ConvertLine {line} {
	set idx [string first "=" $line]
	if { [IsComment $line] } {
		return ""
	}
	if { [IsMode $line] } {
		global mode
		set mode [ExtractMode $line]
		return "#$line"
	}
	if { $idx < 0 } {
		if { [llength $line] > 0 } {
			return "#$line"
		} else {
			return ""
		}
	}
	incr idx -1
	set head [string range $line 0 $idx]
	set head [ExtractWords $head]
	incr idx 2
	set tail [string range $line $idx end]
	set tail [ExtractWords $tail]
	
#	set tail "1$tail"
	
	global mode
	set name [format "%s%s" $mode $head]
	return [format "set %25s \"%s\"" $name $tail]
}

if {$argc} {
	set filename [lindex $argv 0]
} else {
	return 1
}

if { [catch {open $filename} fp] } {
	puts $fp
	return 1
}

set mode ""
while { [gets $fp line] >= 0 } {
	puts [ConvertLine $line]
}
close $fp

return 0