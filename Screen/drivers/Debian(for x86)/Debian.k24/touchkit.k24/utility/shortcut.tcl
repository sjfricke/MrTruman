#!/bin/sh
#\
	exec tcl "$0" "$@"

set PORT ""
set IMAGE_HOME [lindex $argv 0]

set format \
{%s
[%sDesktop Entry]
SwallowExec=
SwallowTitle=
Name=touchcfg
BinaryPattern=
MimeType=
Exec=touchcfg %s
Icon=%s/touch.png
TerminalOptions=
Path=
Type=Application
Terminal=0
}

set InstallMode [string compare $IMAGE_HOME remove]


set HomeList [glob -nocomplain /home/*]
lappend HomeList /root /etc/skel
foreach HOME $HomeList {
	foreach COMMENT {"# KDE Config File"	""} \
		ENV	{"KDE "			""} \
		WHERE	"$HOME/Desktop		$HOME/.gnome-desktop" \
		NAME	{"touchcfg"		"touchcfg.desktop"} 	{
		if { $InstallMode } {
			if { ! [catch {open $WHERE/$NAME "w"} file] } {
				puts $file [format $format $COMMENT $ENV $PORT $IMAGE_HOME]
				close $file
				set tokens [split $HOME "/"]
				set username [lindex $tokens end]
				catch {exec chown $username:$username $WHERE/$NAME}
			}
		} else {
			catch {file delete -force $WHERE/$NAME}
		}
	}
}

