#!/bin/sh
# the next line restarts using wish\
exec wish "$0" "$@" 

if {![info exists vTcl(sourcing)]} {
    switch $tcl_platform(platform) {
	windows {
	}
	default {
	    option add *Scrollbar.width 10
	}
    }
    
}
#############################################################################
# Visual Tcl v1.51 Project
#

#################################
# VTCL LIBRARY PROCEDURES
#

if {![info exists vTcl(sourcing)]} {
proc Window {args} {
    global vTcl
    set cmd     [lindex $args 0]
    set name    [lindex $args 1]
    set newname [lindex $args 2]
    set rest    [lrange $args 3 end]
    if {$name == "" || $cmd == ""} { return }
    if {$newname == ""} { set newname $name }
    if {$name == "."} { wm withdraw $name; return }
    set exists [winfo exists $newname]
    switch $cmd {
        show {
            if {$exists} {
                wm deiconify $newname
            } elseif {[info procs vTclWindow$name] != ""} {
                eval "vTclWindow$name $newname $rest"
            }
            if {[wm state $newname] == "normal"} {
                vTcl:FireEvent $newname <<Show>>
            }
        }
        hide    {
            if {$exists} {
                wm withdraw $newname
                vTcl:FireEvent $newname <<Hide>>
                return}
        }
        iconify { if $exists {wm iconify $newname; return} }
        destroy { if $exists {destroy $newname; return} }
    }
}
}

if {![info exists vTcl(sourcing)]} {
proc {vTcl:DefineAlias} {target alias widgetProc top_or_alias cmdalias} {
    global widget

    set widget($alias) $target
    set widget(rev,$target) $alias

    if {$cmdalias} {
        interp alias {} $alias {} $widgetProc $target
    }

    if {$top_or_alias != ""} {
        set widget($top_or_alias,$alias) $target

        if {$cmdalias} {
            interp alias {} $top_or_alias.$alias {} $widgetProc $target
        }
    }
}

proc {vTcl:DoCmdOption} {target cmd} {
    ## menus are considered toplevel windows
    set parent $target
    while {[winfo class $parent] == "Menu"} {
        set parent [winfo parent $parent]
    }

    regsub -all {\%widget} $cmd $target cmd
    regsub -all {\%top} $cmd [winfo toplevel $parent] cmd

    uplevel #0 [list eval $cmd]
}

proc {vTcl:FireEvent} {target event} {
    foreach bindtag [bindtags $target] {
        set tag_events [bind $bindtag]
        set stop_processing 0
        foreach tag_event $tag_events {
            if {$tag_event == $event} {
                set bind_code [bind $bindtag $tag_event]
                regsub -all %W $bind_code $target bind_code
                set result [catch {uplevel #0 $bind_code} errortext]
                if {$result == 3} {
                    # break exception, stop processing
                    set stop_processing 1
                } elseif {$result != 0} {
                    bgerror $errortext
                }
                break
            }
        }
        if {$stop_processing} {break}
    }
}

proc {vTcl:Toplevel:WidgetProc} {w args} {
    if {[llength $args] == 0} {
        return -code error "wrong # args: should be \"$w option ?arg arg ...?\""
    }

    ## The first argument is a switch, they must be doing a configure.
    if {[string index $args 0] == "-"} {
        set command configure

        ## There's only one argument, must be a cget.
        if {[llength $args] == 1} {
            set command cget
        }
    } else {
        set command [lindex $args 0]
        set args [lrange $args 1 end]
    }

    switch -- $command {
        "hide" -
        "Hide" {
            Window hide $w
        }

        "show" -
        "Show" {
            Window show $w
        }

        "ShowModal" {
            Window show $w
            raise $w
            grab $w
            tkwait window $w
            grab release $w
        }

        default {
            eval $w $command $args
        }
    }
}

proc {vTcl:WidgetProc} {w args} {
    if {[llength $args] == 0} {
        return -code error "wrong # args: should be \"$w option ?arg arg ...?\""
    }

    ## The first argument is a switch, they must be doing a configure.
    if {[string index $args 0] == "-"} {
        set command configure

        ## There's only one argument, must be a cget.
        if {[llength $args] == 1} {
            set command cget
        }
    } else {
        set command [lindex $args 0]
        set args [lrange $args 1 end]
    }

    eval $w $command $args
}

proc {vTcl:toplevel} {args} {
    uplevel #0 eval toplevel $args
    set target [lindex $args 0]
    namespace eval ::$target {}
}
}

if {[info exists vTcl(sourcing)]} {
proc vTcl:project:info {} {
    namespace eval ::widgets::.top18 {
        array set save {}
    }
    namespace eval ::widgets::.top18.fra21 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.lab80 {
        array set save {-padx 1 -relief 1 -takefocus 1 -text 1}
    }
    namespace eval ::widgets::.top18.fra21.lab82 {
        array set save {-padx 1 -pady 1 -relief 1 -takefocus 1 -text 1}
    }
    namespace eval ::widgets::.top18.fra21.fra31 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra31.can36 {
        array set save {-background 1 -borderwidth 1 -closeenough 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra31.fra42 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra31.fra42.but76 {
        array set save {-command 1 -padx 1 -text 1}
    }
    namespace eval ::widgets::.top18.fra21.fra31.fra42.but77 {
        array set save {-command 1 -padx 1 -text 1}
    }
    namespace eval ::widgets::.top18.fra21.fra31.fra42.but73 {
        array set save {-text 1}
    }
    namespace eval ::widgets::.top18.fra21.fra31.fra42.but74 {
        array set save {-text 1}
    }
    namespace eval ::widgets::.top18.fra21.fra31.fra42.but75 {
        array set save {-text 1}
    }
    namespace eval ::widgets::.top18.fra21.fra31.fra72 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra56 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra56.lab59 {
        array set save {-anchor 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra56.sca60 {
        array set save {-bigincrement 1 -command 1 -from 1 -orient 1 -resolution 1 -showvalue 1 -sliderlength 1 -tickinterval 1 -to 1 -variable 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra56.fra61 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra65 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra65.lab59 {
        array set save {-anchor 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra65.sca60 {
        array set save {-bigincrement 1 -command 1 -from 1 -orient 1 -resolution 1 -showvalue 1 -sliderlength 1 -tickinterval 1 -to 1 -variable 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra65.fra61 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra66 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra66.lab59 {
        array set save {-anchor 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra66.sca60 {
        array set save {-bigincrement 1 -command 1 -from 1 -orient 1 -resolution 1 -showvalue 1 -sliderlength 1 -tickinterval 1 -to 1 -variable 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra28.fra66.fra61 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29.fra24 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29.fra24.lab59 {
        array set save {-anchor 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29.fra24.sca60 {
        array set save {-bigincrement 1 -command 1 -from 1 -orient 1 -resolution 1 -showvalue 1 -sliderlength 1 -tickinterval 1 -to 1 -variable 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29.fra24.fra61 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29.fra25 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29.fra25.lab59 {
        array set save {-anchor 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29.fra25.sca60 {
        array set save {-bigincrement 1 -command 1 -from 1 -orient 1 -resolution 1 -showvalue 1 -sliderlength 1 -tickinterval 1 -to 1 -variable 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29.fra25.fra61 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29.fra40 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29.fra40.lab59 {
        array set save {-anchor 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra33.fra29.fra40.fra61 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra25 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra25.fra39 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra25.fra39.lab59 {
        array set save {-anchor 1 -text 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra25.che37 {
        array set save {-anchor 1 -command 1 -text 1 -variable 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra25.che38 {
        array set save {-anchor 1 -command 1 -text 1 -variable 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26.fra56 {
        array set save {-borderwidth 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26.fra56.lab59 {
        array set save {-anchor 1 -text 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26.fra65 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26.fra65.lab59 {
        array set save {-anchor 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26.fra65.sca60 {
        array set save {-bigincrement 1 -command 1 -from 1 -orient 1 -resolution 1 -showvalue 1 -sliderlength 1 -tickinterval 1 -to 1 -variable 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26.fra65.fra61 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26.fra66 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26.fra66.lab59 {
        array set save {-anchor 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26.fra66.sca60 {
        array set save {-bigincrement 1 -command 1 -from 1 -orient 1 -resolution 1 -showvalue 1 -sliderlength 1 -tickinterval 1 -to 1 -variable 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26.fra66.fra61 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra23.fra34.fra26.fra18 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra24 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra24.but23 {
        array set save {-command 1 -padx 1 -text 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra24.but24 {
        array set save {-command 1 -state 1 -text 1}
    }
    namespace eval ::widgets::.top18.fra21.fra19.fra25 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra49 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top18.fra49.but25 {
        array set save {-command 1 -height 1 -padx 1 -text 1}
    }
    namespace eval ::widgets::.top18.fra51 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top19 {
        array set save {}
    }
    namespace eval ::widgets::.top19.fra33 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top19.fra33.fra35 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top19.fra33.fra35.01 {
        array set save {-command 1 -orient 1}
    }
    namespace eval ::widgets::.top19.fra33.fra35.02 {
        array set save {-command 1}
    }
    namespace eval ::widgets::.top19.fra33.fra35.03 {
        array set save {-cursor 1 -font 1 -height 1 -width 1 -wrap 1 -xscrollcommand 1 -yscrollcommand 1}
    }
    namespace eval ::widgets::.top19.fra33.fra36 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top19.fra33.but38 {
        array set save {-command 1 -text 1}
    }
    namespace eval ::widgets::.top41 {
        array set save {}
    }
    namespace eval ::widgets::.top41.fra24 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top41.fra24.lab35 {
        array set save {-padx 1 -pady 1 -text 1}
    }
    namespace eval ::widgets::.top41.fra24.tex36 {
        array set save {-height 1 -padx 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top41.fra24.fra37 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top41.fra25 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top41.fra25.01 {
        array set save {-command 1 -orient 1}
    }
    namespace eval ::widgets::.top41.fra25.02 {
        array set save {-command 1}
    }
    namespace eval ::widgets::.top41.fra25.03 {
        array set save {-font 1 -height 1 -padx 1 -pady 1 -width 1 -xscrollcommand 1 -yscrollcommand 1}
    }
    namespace eval ::widgets::.top41.but26 {
        array set save {-command 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top52 {
        array set save {}
    }
    namespace eval ::widgets::.top63 {
        array set save {}
    }
    namespace eval ::widgets::.top63.fra73 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra73.lab165 {
        array set save {-background 1 -padx 1 -pady 1 -relief 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23.lab24 {
        array set save {-anchor 1 -pady 1 -relief 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23.lab40 {
        array set save {-anchor 1 -pady 1 -relief 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23.lab41 {
        array set save {-anchor 1 -pady 1 -relief 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23.lab42 {
        array set save {-anchor 1 -pady 1 -relief 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23.fra59 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23.fra54 {
        array set save {-background 1 -borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23.lis57 {
        array set save {-background 1 -borderwidth 1 -height 1 -relief 1 -selectbackground 1 -selectborderwidth 1 -width 1 -yscrollcommand 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23.lis58 {
        array set save {-background 1 -borderwidth 1 -height 1 -relief 1 -selectbackground 1 -selectborderwidth 1 -width 1 -yscrollcommand 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23.lis59 {
        array set save {-background 1 -borderwidth 1 -height 1 -relief 1 -selectbackground 1 -selectborderwidth 1 -width 1 -yscrollcommand 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23.lis60 {
        array set save {-background 1 -borderwidth 1 -height 1 -relief 1 -selectbackground 1 -selectborderwidth 1 -width 1 -yscrollcommand 1}
    }
    namespace eval ::widgets::.top63.fra73.fra23.scr25 {
        array set save {-command 1}
    }
    namespace eval ::widgets::.top63.fra73.fra64 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra73.fra64.but104 {
        array set save {-anchor 1 -command 1 -pady 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra73.fra64.but105 {
        array set save {-command 1 -padx 1 -pady 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra73.fra64.but106 {
        array set save {-command 1 -padx 1 -pady 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra18 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra18.fra67 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra18.fra67.lab164 {
        array set save {-background 1 -padx 1 -pady 1 -relief 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra18.fra67.rad72 {
        array set save {-anchor 1 -command 1 -padx 1 -pady 1 -text 1 -variable 1}
    }
    namespace eval ::widgets::.top63.fra18.fra67.rad73 {
        array set save {-anchor 1 -command 1 -padx 1 -pady 1 -text 1 -variable 1}
    }
    namespace eval ::widgets::.top63.fra18.fra67.rad74 {
        array set save {-anchor 1 -command 1 -text 1 -variable 1}
    }
    namespace eval ::widgets::.top63.fra18.lab69 {
        array set save {-padx 1 -pady 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra19 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.lab164 {
        array set save {-background 1 -padx 1 -pady 1 -relief 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra19 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra19.rad20 {
        array set save {-anchor 1 -command 1 -padx 1 -pady 1 -text 1 -variable 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra19.rad21 {
        array set save {-anchor 1 -command 1 -padx 1 -text 1 -variable 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra19.che22 {
        array set save {-command 1 -padx 1 -pady 1 -relief 1 -text 1 -variable 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra30 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra30.lab26 {
        array set save {-anchor 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra30.lab27 {
        array set save {-anchor 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra30.fra28 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra30.fra28.but78 {
        array set save {-command 1 -padx 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra30.fra28.sca80 {
        array set save {-bigincrement 1 -command 1 -from 1 -orient 1 -resolution 1 -showvalue 1 -sliderlength 1 -to 1 -variable 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra30.fra28.but81 {
        array set save {-command 1 -padx 1 -pady 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra30.fra29 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra30.fra29.but78 {
        array set save {-command 1 -padx 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra30.fra29.sca80 {
        array set save {-bigincrement 1 -command 1 -from 1 -orient 1 -resolution 1 -showvalue 1 -sliderlength 1 -tickinterval 1 -to 1 -variable 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra18.fra30.fra29.but81 {
        array set save {-command 1 -height 1 -padx 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra19 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top63.fra19.fra19.but20 {
        array set save {-command 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra19.fra19.but21 {
        array set save {-command 1 -padx 1 -pady 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra19.fra19.but24 {
        array set save {-command 1 -padx 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra19.fra19.but25 {
        array set save {-command 1 -padx 1 -text 1}
    }
    namespace eval ::widgets::.top63.fra19.fra19.but26 {
        array set save {-command 1 -text 1}
    }
    namespace eval ::widgets::.top74 {
        array set save {}
    }
    namespace eval ::widgets::.top74.fra52 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    namespace eval ::widgets::.top74.fra52.fra51 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top74.fra52.fra51.lab48 {
        array set save {-pady 1 -text 1}
    }
    namespace eval ::widgets::.top74.fra52.fra51.ent49 {
        array set save {-background 1 -textvariable 1 -width 1}
    }
    namespace eval ::widgets::.top74.fra19 {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    namespace eval ::widgets::.top74.fra19.but24 {
        array set save {-command 1 -height 1 -pady 1 -text 1 -width 1}
    }
    namespace eval ::widgets::.top74.fra19.but25 {
        array set save {-command 1 -height 1 -text 1 -width 1}
    }
    namespace eval ::widgets_bindings {
        set tagslist {lbPanelList btAdvanced}
    }
}
}
#################################
# USER DEFINED PROCEDURES
#
###########################################################
## Procedure:  Bool_atoi

proc {Bool_atoi} {str} {
if { [scan $str "%d" val] } {
    if { $val } {
        return 1
    } else {
        return 0
    }     
}

set ustr [string toupper $str]

if { ![string compare $ustr "TRUE"] } {
    return 1
}
return 0
}
###########################################################
## Procedure:  DriverPresent

proc {DriverPresent} {} {
global TPANELD_NAME
global PRJNAME
global ${PRJNAME}_LOCK

set lock [subst $${PRJNAME}_LOCK]

if { [catch {exec cat $lock} rval] } {
    puts "DriverPresent: $rval"
    return 0
} else {
    set pid $rval
}

return [expr ![catch {exec kill -0 $pid}]]
}
###########################################################
## Procedure:  GetPanel25PtCoef

proc {GetPanel25PtCoef} {panel} {
global widget
global V_WorkPath V_PanelInfo

if { [info exists V_PanelInfo($panel,2)] } {
    return $V_PanelInfo($panel,2)
}

if { [catch {exec $V_WorkPath/utility/PanelInfo $panel -2} rval] } {
    puts "GetPanelInfo: $rval"
    return ""
}
set list [split $rval "\n"]

set 2 ""
set S ""
set ExpectData 0
foreach item $list {
    if { [string length $item] == 1 } {
        set ExpectData 1
        set var $item
    } else {
        if { $ExpectData } {
            append $var $item
        }
    }
}
if { ![string compare $S "OK"] } {
    set V_PanelInfo($panel,2) $2
    return $2
} else {
    return ""
}
}
###########################################################
## Procedure:  GetPanelInfo

proc {GetPanelInfo} {panel args} {
global widget
global V_WorkPath

if { [catch {exec $V_WorkPath/utility/PanelInfo $panel -tv2p} rval] } {
    puts "GetPanelInfo: $rval"
    global Main_E_unknown
    return [list $panel "" "" $Main_E_unknown]
}

set list [split $rval "\n"]
set V ""
set T ""
set S ""
set 2 ""
set P ""
set ExpectData 0
foreach item $list {
    if { [string length $item] == 1 } {
        set ExpectData 1
        set var $item
    } else {
        if { $ExpectData } {
            append $var $item
        }
    }
}
if { [string compare $S "OK"] } {
    global Main_E_noresponce Main_E_notfound Main_E_notchar Main_E_openfailed
    switch $S {
        "no responce" {set S $Main_E_noresponce}
        "not found" {set S $Main_E_notfound}
#        "not char - device" {set S $Main_E_notchar}
#        "open failed" {set S $Main_E_openfailed}
    }
} else {
    global Main_OK
    set S $Main_OK
    global V_PanelInfo
    set V_PanelInfo($panel,2) $2
    set V_PanelInfo($panel,P) $P 
} 

return "$panel \"$T\" \"$V\" \"$S\""
}
###########################################################
## Procedure:  GetPanelPerfParm

proc {GetPanelPerfParm} {panel} {
global widget
global V_WorkPath V_PanelInfo

if { [info exists V_PanelInfo($panel,P)] } {
    return $V_PanelInfo($panel,P)
}

if { [catch {exec $V_WorkPath/utility/PanelInfo $panel -p} rval] } {
    puts "GetPanelInfo: $rval"
    return ""
}
set list [split $rval "\n"]

set P ""
set S ""
set ExpectData 0
foreach item $list {
    if { [string length $item] == 1 } {
        set ExpectData 1
        set var $item
    } else {
        if { $ExpectData } {
            append $var $item
        }
    }
}
if { ![string compare $S "OK"] } {
    set V_PanelInfo($panel,P) $P
    return $P
} else {
    return ""
}
}
###########################################################
## Procedure:  GetPanelSetting

proc {GetPanelSetting} {} {
global PRJNAME
global ${PRJNAME}_CONF
global V_MouseMode V_DblClickSpeed V_DblClickArea V_RClickTool V_SoundMode
global DefaultMouseMode DefaultDblClickSpeed DefaultDblClickArea DefaultRClickTool DefaultSoundMode

set V_MouseMode $DefaultMouseMode
set V_DblClickSpeed $DefaultDblClickSpeed
set V_DblClickArea $DefaultDblClickArea
set V_RClickTool $DefaultRClickTool
set V_SoundMode $DefaultSoundMode

set conf [subst $${PRJNAME}_CONF]

if { [catch {open $conf "r"} rval] } {
    global DefaultConfData
    puts "GetPanelSetting: $rval"
    if { [catch {exec echo $DefaultConfData > $conf} rval] } {
        puts $rval
    }
    return
}

set file $rval
global KeywordList
set UKeywordList [string toupper $KeywordList]

while { [gets $file line] >= 0 } {
    if {[_IsComment $line]} continue
    set equal [string first "=" $line]
    if {$equal >= 0} {
        set head [string range $line 0 [expr $equal - 1]]
        set head [lindex $head 0]
        set head [string toupper $head]
        set tail "list [string range $line [expr $equal + 1] end]"
        set tail [eval $tail]
        set idx [lsearch $UKeywordList $head]
        if { $idx >= 0 } {
            _SetKeyword $idx $tail
        }
    }
}
close $file

#if RClickTool is ON, get V_MouseMode from driver
if { [string compare "0" $V_RClickTool] } {
	global ${PRJNAME}_CMDIN ${PRJNAME}_CMDOUT
	set cmdin [subst $${PRJNAME}_CMDIN]
	set cmdout [subst $${PRJNAME}_CMDOUT]

   	global AP2D_GETMODE
    if { [catch {exec echo \[$AP2D_GETMODE\][pid] > $cmdin} rval] } {
    	puts $rval
	} else { if { [catch {open $cmdout} file] } {
		puts $file
    } else {
    	fconfigure $file -blocking 0
    	after 200
    	while { [gets $file line] > 0 } {
    		if { [scan $line "M%d" val] } {
    			set V_MouseMode $val
			}
    	}
    } }
}
}
###########################################################
## Procedure:  GetRToolStatus

proc {GetRToolStatus} {} {
global RCLICK_LOCK

if { ![catch {exec cat $RCLICK_LOCK} rval] } {
	#Get PID OK
    set pid [lindex $rval 0]
    set krval ""
	set false [catch {exec kill -0 $pid} krval]
	if { !$false || [string first "permit" $krval] >= 0 } {
        return [lindex $rval 1]
    }
}

return 0
}
###########################################################
## Procedure:  InitAbout

proc {InitAbout} {} {
global widget
global V_WorkPath

global About_Title About_OK
wm title $widget(tlAbout) About_Title
$widget(btAboutOK) configure -text $About_OK

catch {$widget(lbAboutLogo) configure -image AboutImage}

$widget(ttAboutCRight) delete 1.0 end
global About_Copyright
if { [catch { $widget(ttAboutCRight) insert end  [exec cat $V_WorkPath/diag/$About_Copyright] } rval] } {
    puts $rval
}
$widget(ttAboutCRight) configure -state disabled

$widget(ttAboutInfo) delete 1.0 end
global About_Profile
if { [catch { $widget(ttAboutInfo) insert end  [exec cat $V_WorkPath/diag/$About_Profile] } rval] } {
    puts $rval
}
$widget(ttAboutInfo) configure -state disabled

focus $widget(btAboutOK)
}
###########################################################
## Procedure:  InitAddPanel

proc {InitAddPanel} {} {
global widget
global DefaultDeviceList
global _APOKP_err

set _APOKP_err 0

global AddPanel_Title AddPanel_Interface AddPanel_OK AddPanel_Cancel
wm title $widget(tlAddPanel) $AddPanel_Title
$widget(lbAddPanelInterface) configure -text $AddPanel_Interface
$widget(btAddPanelOK) configure -text $AddPanel_OK
$widget(btAddPanelCancel) configure -text $AddPanel_Cancel


#set list [$widget(lbPanelListInterface) get 0 end]
global V_Interface
set list $V_Interface

set auto ""
foreach item $DefaultDeviceList {
    if { [lsearch $list $item] < 0 } {
        set auto $item
        break
    }
}

$widget(etAddPanel) delete 0 end
if { [llength $auto] } {
    $widget(etAddPanel) insert 0 $auto
#    $widget(btAddPanelOK) configure -state normal
} else {
#    $widget(btAddPanelOK) configure -state disabled   
} 
focus $widget(etAddPanel)
$widget(etAddPanel) selection range 0 end
}
###########################################################
## Procedure:  InitAdvanced

proc {InitAdvanced} {} {
global widget
global _IA_GridInfo _IA_TagBtList _IA_TagFrList _IA_TagTxtList

global Advanced_Title Advanced_Close
wm title $widget(tlAdvanced) $Advanced_Title
$widget(btAdvancedClose) configure -text $Advanced_Close

global Advanced_Calibrate Advanced_Clear Advanced_Default Advanced_Apply
$widget(btAdvancedCalibrate) configure -text $Advanced_Calibrate
$widget(btAdvancedClear) configure -text $Advanced_Clear
$widget(btAdvancedDefault) configure -text $Advanced_Default
$widget(btAdvancedApply) configure -text $Advanced_Apply

global g_ParmList
set g_ParmList "AccLimit ADLimit Debounced Fast Slow FilterOn PowerSaveOn LiftCount LiftRange"

#foreach name {A B C D E} {
#    global Advanced_Parameter$name
#    $widget(lbAdvancedParameter$name) configure -text [subst \$Advanced_Parameter$name]
#}

global PerfTuningOn
global User_Adjustment

set PerfTuningOn [Bool_atoi $User_Adjustment]

global Advanced_Cal25Pt Advanced_PerfTune
$widget(lbAdvanced25PCoef) configure -text $Advanced_Cal25Pt
$widget(lbAdvancedPerfTune) configure -text $Advanced_PerfTune
if { $PerfTuningOn } {
    if { ![info exists _IA_GridInfo] } {
        set _IA_GridInfo [grid info $widget(frAdvanced25PCoef)]
        set _IA_TagBtList "$widget(lbAdvanced25PCoef) $widget(lbAdvancedPerfTune)"
        set _IA_TagFrList "$widget(frAdvanced25PCoef) $widget(frAdvancedPerfTune)"
        set _IA_TagTxtList [list $Advanced_Cal25Pt $Advanced_PerfTune]
    }
} else {
    if { ![info exists _IA_GridInfo] } {
        set _IA_GridInfo [grid info $widget(frAdvanced25PCoef)]
        set _IA_TagBtList "$widget(lbAdvanced25PCoef)"
        set _IA_TagFrList "$widget(frAdvanced25PCoef)"
        set _IA_TagTxtList [list $Advanced_Cal25Pt]
        grid forget $widget(lbAdvancedPerfTune)
        grid forget $widget(frAdvancedPerfTune)
    }
}

$widget(lbAdvanced25PCoef) configure -takefocus 1 -relief raised
focus $widget(lbAdvanced25PCoef)

focus $widget(btAdvancedCalibrate)

global V_MaxDimension V_MarkPoint
global _Adv_CvWidth _Adv_CvHeight _Adv_CvBorder
global _Adv_CvGridColor _Adv_CvLineColor _Adv_CvBgColor

if { ![info exists V_MaxDimension] } {
    set _Adv_CvGridColor white
    set _Adv_CvLineColor yellow
    set _Adv_CvBgColor black
    set V_MarkPoint(0) 102
    set V_MarkPoint(1) 512
    set V_MarkPoint(2) 1024
    set V_MarkPoint(3) 1535
    set V_MarkPoint(4) 1945
    set V_MaxDimension 2047.0
    set _Adv_CvBorder [$widget(cvAdvancedCanvas) cget -borderwidth]
    set _Adv_CvWidth [$widget(cvAdvancedCanvas) cget -width]
    set _Adv_CvWidth [expr $_Adv_CvWidth - 2 * $_Adv_CvBorder]
    incr _Adv_CvWidth 2
    set _Adv_CvHeight [$widget(cvAdvancedCanvas) cget -height]
    set _Adv_CvHeight [expr $_Adv_CvHeight - 2 * $_Adv_CvBorder]
    incr _Adv_CvHeight 3    
    
    $widget(cvAdvancedCanvas) configure -background $_Adv_CvBgColor
    _AS_CanvasAdd rect $V_MarkPoint(0) $V_MarkPoint(0) $V_MarkPoint(4) $V_MarkPoint(4) $_Adv_CvGridColor
    _AS_CanvasAdd rect $V_MarkPoint(0) $V_MarkPoint(1) $V_MarkPoint(4) $V_MarkPoint(3) $_Adv_CvGridColor
    _AS_CanvasAdd rect $V_MarkPoint(1) $V_MarkPoint(0) $V_MarkPoint(3) $V_MarkPoint(4) $_Adv_CvGridColor
    _AS_CanvasAdd line $V_MarkPoint(0) $V_MarkPoint(2) $V_MarkPoint(4) $V_MarkPoint(2) $_Adv_CvGridColor
    _AS_CanvasAdd line $V_MarkPoint(2) $V_MarkPoint(0) $V_MarkPoint(2) $V_MarkPoint(4) $_Adv_CvGridColor  
}

_AS_CanvasUpdate

global _Adv_JustClear
set _Adv_JustClear 0

_AS_PerfParmUpdate
}
###########################################################
## Procedure:  InitExecCommand

proc {InitExecCommand} {args} {
global widget
global V_WorkPath

global Exec_Title Exec_OK
wm title $widget(tlExecCommand) $Exec_Title
$widget(btExecCommandOK) configure -text $Exec_OK

    
$widget(btExecCommandOK) configure -state disabled

$widget(ttExecCommand) configure -state normal
    $widget(ttExecCommand) delete 1.0 end
    $widget(ttExecCommand) insert end "\nExec \"$args\"\n"
    $widget(ttExecCommand) insert end "======================================\n"
#    $widget(ttExecCommand) tag delete _IEC_Tag
#    $widget(ttExecCommand) tag add _IEC_Tag 1.0 end
#    $widget(ttExecCommand) tag configure _IEC_Tag -lmargin1 12 -lmargin2 12
$widget(ttExecCommand) configure -state disabled
update

catch "exec $V_WorkPath/diag/$args" rval

$widget(ttExecCommand) configure -state normal
    $widget(ttExecCommand) insert end  $rval
$widget(ttExecCommand) configure -state disabled

$widget(btExecCommandOK) configure -state normal
focus $widget(btExecCommandOK)

return $rval
}
###########################################################
## Procedure:  InitMain

proc {InitMain} {} {
global widget

global Main_Title
wm title $widget(tlMain) $Main_Title

global V_SupportListVar
if { ![catch {$widget(lbPanelListInterface) cget -listvariable}] } {
    $widget(lbPanelListInterface) configure -listvariable V_Interface
    $widget(lbPanelListType) configure -listvariable V_Type
    $widget(lbPanelListVersion) configure -listvariable V_Version
    $widget(lbPanelListStatus) configure -listvariable V_Status
    set V_SupportListVar 1
} else {
    foreach list {Interface Type Version Status} {
        global V_$list
        set V_$list ""
        $widget(lbPanelList$list) configure -selectmode single
    }
    bind lbPanelList <<ListBoxSelect>> ""   
    bind lbPanelList <ButtonRelease-1> {
        set _idx [%W index anchor]
        $widget(lbPanelListInterface) selection anchor $_idx
        $widget(lbPanelListInterface) selection set $_idx
        PanelList select $widget(lbPanelListInterface)
    }    
    set V_SupportListVar 0
}

global User_Advanced
if { ![Bool_atoi $User_Advanced] } {
    pack forget $widget(btAdvanced)
}

catch {$widget(lbLogo) configure -image MainImage} rval

global Main_Interface Main_Type Main_FWVersion Main_Status
global Main_PanelCandidate Main_AddPanel Main_RemovePanel Main_RescanPanel
$widget(lbInterface) configure -text $Main_Interface
$widget(lbType) configure -text $Main_Type
$widget(lbVersion) configure -text $Main_FWVersion
$widget(lbStatus) configure -text $Main_Status
$widget(lbPanelCandidate) configure -text $Main_PanelCandidate
$widget(btPanelListAdd) configure -text $Main_AddPanel
$widget(btPanelListRemove) configure -text $Main_RemovePanel
$widget(btPanelListRescan) configure -text $Main_RescanPanel

global MinDblClickSpeed MaxDblClickSpeed MinDblClickArea MaxDblClickArea
$widget(scDblClickSpeed) configure -from $MinDblClickSpeed -to $MaxDblClickSpeed
$widget(scDblClickArea) configure -from $MinDblClickArea -to $MaxDblClickArea

global Main_MouseMode
$widget(lbMouseMode) configure -text $Main_MouseMode

global TM_DRAWING TM_DESKTOP
global Main_Desktop Main_Drawing
$widget(rbDrawingMode) configure -value $TM_DRAWING -text $Main_Drawing
$widget(rbDesktopMode) configure -value $TM_DESKTOP -text $Main_Desktop

global Main_ModeButton
$widget(cbModeButton) configure -text $Main_ModeButton

global Main_DoubleClickSpeed Main_DoubleClickArea
$widget(lbDblClickSpeed) configure -text $Main_DoubleClickSpeed
$widget(lbDblClickArea) configure -text $Main_DoubleClickArea

global Main_SlowDown Main_SpeedUp Main_ShrinkArea Main_ExpandArea
$widget(btDblClickSpeedSlow) configure -text $Main_SlowDown
$widget(btDblClickSpeedFast) configure -text $Main_SpeedUp
$widget(btDblClickAreaSmall) configure -text $Main_ShrinkArea
$widget(btDblClickAreaLarge) configure -text $Main_ExpandArea

global Main_Sound
$widget(lbSound) configure -text $Main_Sound

global SM_NONE SM_DOWN SM_UP
global Main_NoSound Main_SoundTouchDown Main_SoundTouchUp
$widget(rbNoSound) configure -value $SM_NONE -text $Main_NoSound
$widget(rbTouchDown) configure -value $SM_DOWN -text $Main_SoundTouchDown
$widget(rbLiftUp) configure -value $SM_UP -text $Main_SoundTouchUp

global Main_About Main_Quit Main_Advanced Main_Cal4Pt Main_DrawTest
$widget(btAbout) configure -text $Main_About
$widget(btAdvanced) configure -text $Main_Advanced
$widget(btClose) configure -text $Main_Quit
$widget(bt4PCal) configure -text $Main_Cal4Pt
$widget(btDrawTest) configure -text $Main_DrawTest

$widget(tlMain) configure -cursor watch
GetPanelSetting
$widget(tlMain) configure -cursor ""

global V_OldMouseMode V_MouseMode
set V_OldMouseMode $V_MouseMode
_MouseModeChange

global V_OldSoundMode V_SoundMode
set V_OldSoundMode $V_SoundMode

if { ![string compare "normal" [$widget(btPanelListRescan) cget -state]] } {
    update
    after 300
    $widget(btPanelListRescan) invoke
}

after 1000 {
    global V_RClickTool
    set V_RClickTool [GetRToolStatus]
}


global TOUCHCFG_CMD
if { [catch {open $TOUCHCFG_CMD r+} channel] } {
    puts $channel
} else {
    fileevent $channel readable "onChannelCmd $channel"
}
}
###########################################################
## Procedure:  PanelList

proc {PanelList} {Cmd args} {
global widget
global V_Interface V_Type V_Version V_Status
global V_SupportListVar

global _PL_level
if { ![info exists _PL_level] } {
    set _PL_level 1
} else {
    incr _PL_level
} 

switch $Cmd {
    select {
        _ListboxSelect [lindex $args 0]
    }
    view {
        set idx [lindex $args 0]
        foreach list {Interface Type Version Status} {
            $widget(lbPanelList$list) yview $idx
        }        
    }
    mark {
        set idx [lindex $args 0]
        $widget(lbPanelListInterface) selection clear 0 end      
        $widget(lbPanelListInterface) selection set $idx
        _ListboxSelect $widget(lbPanelListInterface)
    }
    update {       
        set idx [lsearch $V_Interface [lindex $args 0]]
        if { $idx == -1 } {
            set cmd "PanelList add $args"
            eval $cmd
        } else {
            foreach list {Interface Type Version Status} new $args {
                set V_$list [lreplace [subst \$V_$list] $idx $idx $new]
                if { !$V_SupportListVar } {
                    $widget(lbPanelList$list) delete $idx
                    $widget(lbPanelList$list) insert $idx $new
                }      
            }
        }
    }
    add {
        foreach list {Interface Type Version Status} new $args {
            lappend V_$list $new
            if { !$V_SupportListVar } {
                $widget(lbPanelList$list) insert end $new
            }      
        }      
        PanelList view end              
        PanelList mark end
    }
    remove {
        set v [$widget(lbPanelListInterface) index active]
        foreach list {Interface Type Version Status} {
            $widget(lbPanelList$list) delete $v
            if { !$V_SupportListVar } {
                set V_$list [lreplace [subst \$V_$list] $v $v]
            }      
        }              
        if { $v == [$widget(lbPanelListInterface) size] && $v > 0 } {
            incr v -1
        }
        PanelList mark $v
    }
    rescan {
        set idx -1
        foreach V $V_Interface {
            incr idx
            if { ![file exists $V] } {
                global Main_E_notfound
                PanelList update $V "" "" $Main_E_notfound
                continue
            }
            file stat $V result
            if { [string compare $result(type) "characterSpecial"] } {
                global Main_E_notchar
                PanelList update $V "" "" $Main_E_notchar                
                continue
            }
            global Main_Testing
            PanelList update $V "" "" $Main_Testing
            PanelList mark $idx
            PanelList view $idx
            update
            set cmd "PanelList update [GetPanelInfo $V]"
            eval $cmd
        }
        set idx -1
        foreach V $V_Status {
            incr idx
            global Main_OK
            if { ![string compare $V $Main_OK] } {
                PanelList mark $idx
                PanelList view $idx
                SavePanelSetting
                break
            }
        }
    }
}

if { $_PL_level == 1 } {
    set scan 0
    foreach V $V_Interface {
        if { ![file exists $V] } continue
        if { [string compare [file type $V] "characterSpecial"] } continue
        set scan 1; break
    }
    if {$scan} {
        $widget(btPanelListRescan) configure -state normal
    } else {
        $widget(btPanelListRescan) configure -state disabled
    }
    set idx [$widget(lbPanelListStatus) index active]
    global Main_OK
    if { ![string compare [lindex $V_Status $idx] $Main_OK] } {
        global V_CurInterface
        set V_CurInterface [lindex $V_Interface $idx]
        _DiagButtons on
    } else {
        _DiagButtons off
    }
}

incr _PL_level -1
}
###########################################################
## Procedure:  PopMainWindow

proc {PopMainWindow} {} {
global widget
global _SMW_WindowList

set len [llength $_SMW_WindowList]
if { $len == 0 } return

set cur [lindex $_SMW_WindowList end]
grab release $cur

if { $len > 1 } {
    set _SMW_WindowList [lrange $_SMW_WindowList 0 [incr len -2]]
    set old [lindex $_SMW_WindowList end]
    wm protocol $old WM_TAKE_FOCUS ""
    wm protocol $old WM_DELETE_WINDOW "
        PopMainWindow
    "
    Window show $old
    raise $old
    grab set $old
} else {
    SavePanelSetting
    exit
}
Window hide $cur
}
###########################################################
## Procedure:  PushMainWindow

proc {PushMainWindow} {new args} {
global widget
global _SMW_WindowList

global _PMW_$new
if { [info exists _PMW_$new] } {
	raise $new
} else {
	set _PMW_$new ""
}
Window show $new
wm protocol $new WM_DELETE_WINDOW "
    PopMainWindow
"
grab set $new
focus $new

lappend _SMW_WindowList $new
set len [llength $_SMW_WindowList]
if { $len > 1 } {
    incr len -2
    set oldlist [lrange $_SMW_WindowList 0 $len]
    foreach old $oldlist {
        wm protocol $old WM_TAKE_FOCUS "
            raise $new $old
            bell
        "
        wm protocol $old WM_DELETE_WINDOW "
            Window hide $old    
        "
    }
}

if { [llength $args] } {
    eval [lindex $args 0]
}
}
###########################################################
## Procedure:  SavePanelSetting

proc {SavePanelSetting} {} {
global _MouseMode _DblClickSpeed _DblClickArea _RClickTool _Port _Sound
global V_MouseMode V_DblClickSpeed V_DblClickArea V_RClickTool V_SoundMode
global ModeList SoundModeList
global V_Interface

foreach m $ModeList {
    global TM_$m sTM_$m
    set MM([subst \$TM_$m]) [subst \$sTM_$m]
}



set dat "
$_MouseMode = $MM($V_MouseMode)
$_DblClickSpeed = $V_DblClickSpeed
$_DblClickArea = $V_DblClickArea
$_RClickTool = $V_RClickTool
$_Sound = [lindex $SoundModeList $V_SoundMode]

"

foreach device $V_Interface {
    append dat "$_Port = $device\n"
}

global PRJNAME
global ${PRJNAME}_CONF
set conf [subst $${PRJNAME}_CONF]

global V_WorkPath
if { [catch {exec $V_WorkPath/utility/WriteConf $dat} rval] } {
    puts "SavePanelSetting: $rval"
}

#if { [catch {exec echo $dat > $conf} rval] } {
#    puts "SavePanelSetting: $rval"
#}

global TPANELD_NAME
catch { exec killall -HUP $TPANELD_NAME }
}
###########################################################
## Procedure:  SetWindowCursor

proc {SetWindowCursor} {cursor} {
global widget
global _SMW_WindowList

foreach w $_SMW_WindowList {
    $w configure -cursor $cursor
}
}
###########################################################
## Procedure:  UpdateDriverMode

proc {UpdateDriverMode} {args} {
global PRJNAME
global ${PRJNAME}_CMDIN

set cmdin [subst $${PRJNAME}_CMDIN]             

set cmd [lindex $args 0]

global _MouseMode _DblClickSpeed _DblClickArea _Sound


    if { ![string compare $_MouseMode $cmd] } {
        global AP2D_SETMODE V_MouseMode
        if { [catch {exec echo \[$AP2D_SETMODE\]$V_MouseMode > $cmdin} rval] } {
            puts "UpdateDriverMode ($cmd) $rval"
        }
        return 0
    }

    if { ![string compare $_DblClickSpeed $cmd] } {
        global AP2D_SETDCSPEED V_DblClickSpeed
        if { [catch {exec echo \[$AP2D_SETDCSPEED\]$V_DblClickSpeed > $cmdin} rval] } {
            puts "UpdateDriverMode ($cmd) $rval"
        }
        return 0
    }

    if { ![string compare $_DblClickArea $cmd] } {
        global AP2D_SETDCAREA V_DblClickArea
        if { [catch {exec echo \[$AP2D_SETDCAREA\]$V_DblClickArea > $cmdin} rval] } {
            puts "UpdateDriverMode ($cmd) $rval"
        }
        return 0
    }
    
    if { ![string compare $_Sound $cmd] } {
        global AP2D_SETSOUNDMODE V_SoundMode
        if { [catch {exec echo \[$AP2D_SETSOUNDMODE\]$V_SoundMode > $cmdin} rval] } {
            puts "UpdateDriverMode ($cmd) $rval"
        }
        return 0
    }
}
###########################################################
## Procedure:  WinExecCommand

proc {WinExecCommand} {mode args} {
global widget

PushMainWindow $widget(tlExecCommand)

SetWindowCursor watch

#Window hide $widget(tlExecCommand)
update

#$widget(tlExecCommand) configure -cursor watch
set cmd "InitExecCommand $args"
set rval [string compare [eval $cmd] "OK"]
#$widget(tlExecCommand) configure -cursor ""

Window show $widget(tlExecCommand)

if { [string compare $mode wait] && !$rval } {
    PopMainWindow
}

SetWindowCursor ""

return $rval
}
###########################################################
## Procedure:  _AS_ApplyPressed

proc {_AS_ApplyPressed} {} {
global widget
global V_PanelInfo V_CurInterface
global g_ParmList

#global V_AccLimit V_ADLimit V_Debounced V_Fast V_Slow V_FilterOn V_PowerSaveOn V_LiftCount V_LiftRange
#set V_PanelInfo($V_CurInterface,P)  [format "<%d %d %d %d %d %d %d %d %d>"  $V_AccLimit $V_ADLimit $V_Debounced $V_Fast $V_Slow $V_FilterOn $V_PowerSaveOn $V_LiftCount $V_LiftRange]       
#set dat [format "_%d_%d_%d_%d_%d_%d_%d_%d_%d"  $V_AccLimit $V_ADLimit $V_Debounced $V_Fast $V_Slow $V_FilterOn $V_PowerSaveOn $V_LiftCount $V_LiftRange]

set parmList ""
foreach p $g_ParmList {
	global V_$p
	lappend parmList [subst \$V_$p]
}
set cmd "format {<%d %d %d %d %d %d %d %d %d>} $parmList"
set V_PanelInfo($V_CurInterface,P) [eval $cmd]
set cmd "format {_%d_%d_%d_%d_%d_%d_%d_%d_%d} $parmList"
set dat [eval $cmd]

$widget(tlAdvanced) configure -cursor watch
PushMainWindow $widget(tlTesting)
global V_WorkPath                
set val [catch {exec $V_WorkPath/utility/PanelInfo $V_CurInterface -P $dat} rval]
PopMainWindow
$widget(tlAdvanced) configure -cursor ""

if { $val } {
    bell
    puts "GetPanelInfo: $rval"
    return
} else {
    set list [split $rval "\n"]
    set S ""
    set W ""
    set ExpectData 0
    foreach item $list {
            if { [string length $item] == 1 } {
                set ExpectData 1
                set var $item
            } else {
                if { $ExpectData } {
                append $var $item
            }
        }
    }
    if { [string compare $S "OK"] || [string compare $W "<0>"] } {
        bell
    } else {
        $widget(btAdvancedApply) configure -state disabled
    } 
}
}
###########################################################
## Procedure:  _AS_Button1

proc {_AS_Button1} {which} {
global widget

$which configure -takefocus 1 -relief raised
focus $which
}
###########################################################
## Procedure:  _AS_CanvasAdd

proc {_AS_CanvasAdd} {type ux uy lx ly color} {
global widget
global V_MaxDimension
global _Adv_CvWidth _Adv_CvHeight _Adv_CvBorder

set cmd "$widget(cvAdvancedCanvas) create $type"
lappend cmd [expr ( $ux ) * $_Adv_CvWidth / $V_MaxDimension + $_Adv_CvBorder]
lappend cmd [expr ( $uy ) * $_Adv_CvHeight / $V_MaxDimension + $_Adv_CvBorder]
lappend cmd [expr ( $lx ) * $_Adv_CvWidth / $V_MaxDimension + $_Adv_CvBorder]
lappend cmd [expr ( $ly ) * $_Adv_CvHeight / $V_MaxDimension + $_Adv_CvBorder]

switch $type {
rect {
    append cmd " -outline $color"
    }
line {
    append cmd " -fill $color"
    }
}

return [eval $cmd]
}
###########################################################
## Procedure:  _AS_CanvasUpdate

proc {_AS_CanvasUpdate} {} {
global widget
global V_CurInterface
global V_25PtCoef

$widget(tlAdvanced) configure -cursor watch
$widget(tlMain) configure -cursor watch
update
#after 1000
set V_25PtCoef($V_CurInterface) [GetPanel25PtCoef $V_CurInterface]
$widget(tlAdvanced) configure -cursor ""
$widget(tlMain) configure -cursor ""

global _Adv_CvLineColor

if { [llength $V_25PtCoef($V_CurInterface)] } {
    _AS_DrawGrid $V_25PtCoef($V_CurInterface) $_Adv_CvLineColor
} else {
    unset V_25PtCoef($V_CurInterface)
}
}
###########################################################
## Procedure:  _AS_DrawGrid

proc {_AS_DrawGrid} {coef color} {
global widget
global V_MarkPoint
global _ASDG_ObjList

set delta [lrange $coef 1 25]

if { [info exists _ASDG_ObjList] } {
    set cmd "$widget(cvAdvancedCanvas) delete $_ASDG_ObjList"
    eval $cmd
    unset _ASDG_ObjList
}

for {set idx 0} {$idx < 25} {incr idx} {
    set dat [split [lindex $delta $idx] "\[\](),<>"]
    set i [expr $idx / 5]
    set j [expr $idx % 5]
    set X($i,$j) [expr $V_MarkPoint($j) + [lindex $dat 1]]
    set Y($i,$j) [expr $V_MarkPoint($i) + [lindex $dat 2]]
}
 
for {set i 0} {$i < 5} {incr i} {
    for {set j 0} {$j < 5} {incr j} {
        if {$i < 4} {
            set ni [expr $i + 1]
            set obj [_AS_CanvasAdd line $X($i,$j) $Y($i,$j) $X($ni,$j) $Y($ni,$j) $color]
            lappend _ASDG_ObjList $obj
        }
        if {$j < 4} {
            set nj [expr $j + 1]
            set obj [_AS_CanvasAdd line $X($i,$j) $Y($i,$j) $X($i,$nj) $Y($i,$nj) $color]
            lappend _ASDG_ObjList $obj
        }
    }
}
}
###########################################################
## Procedure:  _AS_FocusChange

proc {_AS_FocusChange} {which dir} {
global _IA_GridInfo _IA_TagBtList _IA_TagFrList _IA_TagTxtList

if { $dir > 0 } {
    foreach fr $_IA_TagFrList bt $_IA_TagBtList txt $_IA_TagTxtList {
        if { [string compare $which $bt] } {
            $bt configure -takefocus 0 -relief groove
            grid forget $fr
        } else {
            $bt configure -text "< $txt >" -padx 0
            set cmd "grid configure $fr $_IA_GridInfo"
            eval $cmd
        } 
    }
} else {
    set idx [lsearch $_IA_TagBtList $which]
    $which configure -text "  [lindex $_IA_TagTxtList $idx]  " -padx 3
}
}
###########################################################
## Procedure:  _AS_KeyLorR

proc {_AS_KeyLorR} {which dir} {
global _IA_GridInfo _IA_TagBtList _IA_TagFrList

set idx [lsearch $_IA_TagBtList $which]
set cnt [llength $_IA_TagBtList]

set nidx [expr $idx + $dir]

if { 0 <= $nidx && $nidx < $cnt } {
    set nbt [lindex $_IA_TagBtList $nidx]
    $nbt configure -takefocus 1 -relief raised
    focus $nbt
}
}
###########################################################
## Procedure:  _AS_PerfParmUpdate

proc {_AS_PerfParmUpdate} {} {
global widget
global V_CurInterface

$widget(tlAdvanced) configure -cursor watch
$widget(tlMain) configure -cursor watch
update
after 300

global g_ParmList
set varList $g_ParmList
set parm [GetPanelPerfParm $V_CurInterface]
if { [llength $parm] } {
    set list [split $parm "<> "]
    set list [lrange $list 1 [llength $varList]]
    foreach dat $list var $varList {
        global V_$var
        set V_$var $dat
#        puts "set V_$var $dat"
    }
} else {
    foreach var $varList {
        global V_$var EPR_Default$var
        set V_$var [subst \$EPR_Default$var]
    }
}

$widget(tlAdvanced) configure -cursor ""
$widget(tlMain) configure -cursor ""
}
###########################################################
## Procedure:  _AS_ScaleChange

proc {_AS_ScaleChange} {which where args} {
global widget
global _changed_$which

if { ![info exists _changed_$which] && ![string compare "$args" ""] } {
    set _changed_$which 0
} else {
    $widget(btAdvancedApply) configure -state normal
}
}
###########################################################
## Procedure:  _AddPanelOKPressed

proc {_AddPanelOKPressed} {} {
global widget
global V_etAddPanel
global _APOKP_err

set V_etAddPanel [lindex $V_etAddPanel 0]

if { $_APOKP_err } {
    set V_etAddPanel ""
    set _APOKP_err 0
    return
}

if { ![string compare "" $V_etAddPanel] } {
    bell
    return
}
 
if { ![file exists $V_etAddPanel] } {
    bell
    set _APOKP_err 2
    global AddPanel_E_notfound
    set V_etAddPanel $AddPanel_E_notfound
    $widget(etAddPanel) selection range 0 end
    return
}

if { [string compare [file type $V_etAddPanel] "characterSpecial"] } {
    bell
    set _APOKP_err 2
    global AddPanel_E_notchar
    set V_etAddPanel $AddPanel_E_notchar
    $widget(etAddPanel) selection range 0 end
    return
}

global V_Interface
if { [lsearch $V_Interface $V_etAddPanel] >= 0 } {
    bell
    set _APOKP_err 2
    global AddPanel_E_exist
    set V_etAddPanel $AddPanel_E_exist
    $widget(etAddPanel) selection range 0 end
    return  
}

PopMainWindow
PanelList add $V_etAddPanel "" "" "?"
}
###########################################################
## Procedure:  _DiagButtons

proc {_DiagButtons} {Cmd} {
global widget

set list "DrawTest 4PCal Advanced"

switch $Cmd {
    on {
        foreach item $list {
            $widget(bt$item) configure -state normal        
        }                
    }
    off {
        foreach item $list {
            $widget(bt$item) configure -state disabled        
        }                
    }
}
}
###########################################################
## Procedure:  _IsComment

proc {_IsComment} {line} {
set first [lindex $line 0]

return [expr [string first "#" $first] == 0]
}
###########################################################
## Procedure:  _ListboxSelect

proc {_ListboxSelect} {owner} {
global widget

set list [$owner curselection]
if { [llength $list] != 1 } {
    return
}

if { ![info exists _FO_FG] } {
    global _FO_FG _FO_BG _FO_SFG _FO_SBG
#    set _FO_FG [lindex [$owner configure -foreground] 4]
#    set _FO_BG [lindex [$owner configure -background] 4]
    set _FO_FG [$owner cget -foreground]
    set _FO_BG [$owner cget -background]
    set _FO_SFG Black
    set _FO_SBG #C3C3C3
}

global V_SupportListVar
if { $V_SupportListVar } {
    foreach lb "Interface Type Version Status" {
        set w $widget(lbPanelList$lb)
        set line [$w index active]
        $w itemconfigure $line -background $_FO_BG -foreground $_FO_FG
        $w activate $list
        set line [$w index active]
        $w itemconfigure $line -background $_FO_SBG -foreground $_FO_SFG
    }
} else {
    foreach lb "Interface Type Version Status" {
        set w $widget(lbPanelList$lb)
        $w activate $list
    }
    $widget(lbPanelListInterface) selection anchor $list
    $widget(lbPanelListInterface) selection set $list
}
}
###########################################################
## Procedure:  _MainScale

proc {_MainScale} {cmd} {
global widget

switch $cmd {
    enable {
        $widget(btDblClickSpeedSlow) configure -state normal
        $widget(btDblClickSpeedFast) configure -state normal
        $widget(scDblClickSpeed) configure -state normal -troughcolor #c3c3c3
        $widget(lbDblClickSpeed) configure -foreground black    
        $widget(btDblClickAreaSmall) configure -state normal
        $widget(btDblClickAreaLarge) configure -state normal
        $widget(scDblClickArea) configure -state normal -troughcolor #c3c3c3
        $widget(lbDblClickArea) configure -foreground black       
    }
    disable {
        $widget(btDblClickSpeedSlow) configure -state disabled
        $widget(btDblClickSpeedFast) configure -state disabled
        $widget(scDblClickSpeed) configure -state disabled -troughcolor #d9d9d9
        $widget(lbDblClickSpeed) configure -foreground #a3a3a3      
        $widget(btDblClickAreaSmall) configure -state disabled
        $widget(btDblClickAreaLarge) configure -state disabled
        $widget(scDblClickArea) configure -state disabled -troughcolor #d9d9d9
        $widget(lbDblClickArea) configure -foreground #a3a3a3
    }
}
}
###########################################################
## Procedure:  _MouseModeChange

proc {_MouseModeChange} {} {
global V_OldMouseMode V_MouseMode
global _MouseMode
global TM_DRAWING

if { $V_MouseMode == $TM_DRAWING } {
    _MainScale disable
} else {
    _MainScale enable
} 

if { $V_OldMouseMode != $V_MouseMode } {
    UpdateDriverMode $_MouseMode
}
set V_OldMouseMode $V_MouseMode
}
###########################################################
## Procedure:  _RToolChange

proc {_RToolChange} {} {
global PRJNAME
global ${PRJNAME}_CMDIN

set cmdin [subst $${PRJNAME}_CMDIN]             

global V_RClickTool

if {$V_RClickTool} {
    global AP2D_RCLICKON
    #if { [catch {exec echo \[$AP2D_RCLICKON\][pid] > $cmdin} rval] } {
    #	puts $rval
    #}
} else {
    global AP2D_RCLICKOFF
    if { [catch {exec echo \[$AP2D_RCLICKOFF\][pid] > $cmdin} rval] } {
    	puts $rval
    }
}
}
###########################################################
## Procedure:  _ScrollPanelList

proc {_ScrollPanelList} {args} {
global widget

set cmd1 "$widget(lbPanelListInterface) yview $args"
set cmd2 "$widget(lbPanelListType) yview $args"
set cmd3 "$widget(lbPanelListVersion) yview $args"
set cmd4 "$widget(lbPanelListStatus) yview $args"
eval $cmd1
eval $cmd2
eval $cmd3
eval $cmd4
}
###########################################################
## Procedure:  _SetKeyword

proc {_SetKeyword} {wordidx value} {
global KeywordList _MouseMode _Sound _Port

set modeidx [lsearch $KeywordList $_MouseMode]
if { $wordidx == $modeidx } {
    global V_MouseMode
    global ModeList
    set mode [string toupper $value]
    set modelist [string toupper $ModeList]
    if { [scan $mode "%s" m] } {   
        set idx [lsearch $modelist $m]
        if { $idx >= 0 } {
            set V_MouseMode $idx
        } 
    }
    return
}

set modeidx [lsearch $KeywordList $_Sound]
if { $wordidx == $modeidx } {
    global V_SoundMode
    global SoundModeList
    set mode [string toupper $value]
    set soundmodelist [string toupper $SoundModeList]
    if { [scan $mode "%s" m] } {
        set idx [lsearch $soundmodelist $m]
        if { $idx >= 0 } {
            set V_SoundMode $idx
        } 
    }
    return
}


set portidx [lsearch $KeywordList $_Port]
if { $wordidx == $portidx } {
    set status "?"
    if { ![file exists $value] } {
        global Main_E_notfound
        set status $Main_E_notfound
    } else {
        file stat $value result
        if { [string compare $result(type) "characterSpecial"] } {
            global Main_E_notchar
            set status $Main_E_notchar
        }
    }
    PanelList update $value "" "" $status
} else {
    set name [lindex $KeywordList $wordidx]
    if { [scan $value "%d" v] } {
        global V_$name
        set cmd "set V_$name $v"
        eval $cmd
    } 
}
}
###########################################################
## Procedure:  _SoundModeChange

proc {_SoundModeChange} {} {
global V_OldSoundMode V_SoundMode
global _Sound

if { $V_OldSoundMode != $V_SoundMode } {
    UpdateDriverMode $_Sound
}

set V_OldSoundMode $V_SoundMode
}
###########################################################
## Procedure:  _TakeFocus

proc {_TakeFocus} {args} {
global widget

puts "_TF $args"
}
###########################################################
## Procedure:  _YScrollCmd

proc {_YScrollCmd} {owner args} {
global widget

set cmd "$widget(sbPanelList) set $args"
eval $cmd

set dest [lindex [$widget(sbPanelList) get] 0]

foreach lb "Interface Type Version Status" {
    if { [string first $owner $lb] < 0 } {
        $widget(lbPanelList$lb) yview moveto $dest
    }
}

#puts "YSC: \"($owner)xxx set $args\""
}
###########################################################
## Procedure:  onChannelCmd

proc {onChannelCmd} {channel} {
global widget

if { [gets $channel line] < 0 } {
    fileevent $channel readable {}
    after 500 "close $channel"
} else {
	if { [info exists widget(rb${line}Mode)] } {
		$widget(rb${line}Mode) invoke
	} else { if { ![string compare $line "rescan"] } {
		$widget(btPanelListRescan) invoke
	} }
}
}
###########################################################
## Procedure:  init
###########################################################
## Procedure:  main

proc {main} {argc argv} {
global widget

if { $argc > 0 } {
    global V_WorkPath
    set V_WorkPath [lindex $argv 0]
} else {
    puts "argc: $argc, argv: \"$argv\"\n"
    puts "\nSyntax: touchcfg.tcl <work_path>\n"
    exit
    #for VTcl
    return 0
}

CheckUniqueness

if { [catch {source $V_WorkPath/utility/library.tcl} rval] } {
    puts "<$rval>"
    exit
}

if { [catch {LoadGlobalSetting ${V_WorkPath}/include}] } {
    puts "Can not load global settings"
    exit
}

if { ![DriverPresent] } {
    global TPANELD_NAME
    if { [catch {exec $V_WorkPath/$TPANELD_NAME} rval] } {
        puts $rval
        exit
    }
}

global Main_LogoGif About_LogoGif
if { [catch {image create photo MainImage  -file $V_WorkPath/image/$Main_LogoGif} rval] } {
    puts "image: $rval"
}
if { [catch {image create photo AboutImage  -file $V_WorkPath/image/$About_LogoGif} rval] } {
    puts "image: $rval"
}
        
global TOUCHCFG_CMD
if { [file exists $TOUCHCFG_CMD] } {
    if { [string compare [file type $TOUCHCFG_CMD] "fifo"] } {
        if { [catch {file delete -force $TOUCHCFG_CMD} rval] } {
            puts $rval
        }
    }        
}
if { ![file exists $TOUCHCFG_CMD] } {
    if { [catch {exec mknod -m 666 $TOUCHCFG_CMD p} rval] } {
        puts $rval
    }
}

PushMainWindow $widget(tlMain) InitMain
}
###########################################################
## Procedure:  CheckUniqueness

proc {CheckUniqueness} {} {
set lock    /tmp/.touchcfg.lock

set rval [catch {exec cat $lock} pidStr]

if { !$rval && [scan $pidStr "%d" pid] == 1 } {
	set rval ""
	set false [catch {exec kill -0 $pid} rval]
	if { !$false || [string first "permit" $rval] >= 0 } {
        puts "Another instance ($pid) is running"
        exit
    }
}

catch {exec echo [pid] > $lock} rval
catch {exec chmod 666 $lock}
}

proc init {argc argv} {

}

init $argc $argv

#################################
# VTCL GENERATED GUI PROCEDURES
#

proc vTclWindow. {base {container 0}} {
    if {$base == ""} {
        set base .
    }
    ###################
    # CREATING WIDGETS
    ###################
    if {!$container} {
    wm focusmodel $base passive
    wm geometry $base 1x1+0+0; update
    wm maxsize $base 1137 834
    wm minsize $base 1 1
    wm overrideredirect $base 0
    wm resizable $base 1 1
    wm withdraw $base
    wm title $base "vtcl.tcl"
    bindtags $base "$base Vtcl.tcl all"
    vTcl:FireEvent $base <<Create>>
    }
    ###################
    # SETTING GEOMETRY
    ###################

    vTcl:FireEvent $base <<Ready>>
}

proc vTclWindow.top18 {base {container 0}} {
    if {$base == ""} {
        set base .top18
    }
    if {[winfo exists $base] && (!$container)} {
        wm deiconify $base; return
    }

    global widget
    vTcl:DefineAlias "$base" "tlAdvanced" vTcl:Toplevel:WidgetProc "" 1
    vTcl:DefineAlias "$base.fra21" "Frame57" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19" "frAdvancedPerfTune" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23" "Frame135" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33" "Frame111" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28" "Frame78" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra56" "Frame81" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra56.fra61" "Frame82" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra56.lab59" "lbAdvancedParameterA" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra56.sca60" "Scale4" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra65" "Frame90" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra65.fra61" "Frame89" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra65.lab59" "lbAdvancedParameterB" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra65.sca60" "Scale7" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra66" "Frame92" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra66.fra61" "Frame91" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra66.lab59" "lbAdvancedParameterC" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra28.fra66.sca60" "Scale8" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29" "Frame110" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29.fra24" "Frame94" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29.fra24.fra61" "Frame93" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29.fra24.lab59" "lbAdvancedParameterD" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29.fra24.sca60" "Scale9" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29.fra25" "Frame96" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29.fra25.fra61" "Frame95" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29.fra25.lab59" "lbAdvancedParameterE" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29.fra25.sca60" "Scale10" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29.fra40" "Frame139" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29.fra40.fra61" "Frame138" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra33.fra29.fra40.lab59" "Label10" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34" "Frame133" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra25" "Frame116" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra25.che37" "Checkbutton1" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra25.che38" "Checkbutton2" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra25.fra39" "Frame137" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra25.fra39.lab59" "Label9" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26" "Frame124" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26.fra18" "Frame141" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26.fra56" "Frame119" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26.fra56.lab59" "Label2" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26.fra65" "Frame121" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26.fra65.fra61" "Frame120" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26.fra65.lab59" "Label3" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26.fra65.sca60" "Scale13" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26.fra66" "Frame123" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26.fra66.fra61" "Frame122" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26.fra66.lab59" "Label4" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra23.fra34.fra26.fra66.sca60" "Scale14" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra24" "Frame100" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra24.but23" "btAdvancedDefault" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra24.but24" "btAdvancedApply" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra19.fra25" "Frame142" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra31" "frAdvanced25PCoef" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra31.can36" "cvAdvancedCanvas" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra31.fra42" "Frame75" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra31.fra42.but73" "Button51" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra31.fra42.but74" "Button39" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra31.fra42.but75" "Button46" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra31.fra42.but76" "btAdvancedClear" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra31.fra42.but77" "btAdvancedCalibrate" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.fra31.fra72" "Frame99" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.lab80" "lbAdvanced25PCoef" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra21.lab82" "lbAdvancedPerfTune" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra49" "Frame53" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra49.but25" "btAdvancedClose" vTcl:WidgetProc "tlAdvanced" 1
    vTcl:DefineAlias "$base.fra51" "Frame76" vTcl:WidgetProc "tlAdvanced" 1

    ###################
    # CREATING WIDGETS
    ###################
    if {!$container} {
    vTcl:toplevel $base -class Toplevel
    wm withdraw $base
    wm focusmodel $base passive
    wm geometry $base 352x368+217+138; update
    wm maxsize $base 1009 738
    wm minsize $base 1 1
    wm overrideredirect $base 0
    wm resizable $base 0 0
    wm title $base "Advanced Settings"
    vTcl:FireEvent $base <<Create>>
    }
    frame $base.fra21 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra21.lab80 \
        -padx 0 -relief raised -takefocus 1 -text {< 25-Pt Coef >} 
    bindtags $base.fra21.lab80 "$base.fra21.lab80 btAdvanced Label $base all"
    label $base.fra21.lab82 \
        -padx 3 -pady 1 -relief groove -takefocus 1 -text {  Perf Tuning  } 
    bindtags $base.fra21.lab82 "$base.fra21.lab82 btAdvanced Label $base all"
    frame $base.fra21.fra31 \
        -borderwidth 2 -height 75 -relief raised -width 125 
    canvas $base.fra21.fra31.can36 \
        -background Black -borderwidth 2 -closeenough 1.0 -height 206 \
        -width 310 
    frame $base.fra21.fra31.fra42 \
        -borderwidth 2 -height 75 -width 125 
    button $base.fra21.fra31.fra42.but76 \
        \
        -command if\ \{\ !\ \$_Adv_JustClear\ \}\ \{\n\ \ \ \ \$widget(tlAdvanced)\ configure\ -cursor\ watch\n\ \ \ \ if\ \{\ !\[WinExecCommand\ nowait\ \$_25PCAL_NAME\ \$V_CurInterface\ -0\]\ \}\ \{\n\ \ \ \ \ \ \ \ set\ V_PanelInfo(\$V_CurInterface,2)\ \\\n\"\\\[0\\\]\n(0,0)\ (0,0)\ (0,0)\ (0,0)\ (0,0)\n(0,0)\ (0,0)\ (0,0)\ (0,0)\ (0,0)\n(0,0)\ (0,0)\ (0,0)\ (0,0)\ (0,0)\n(0,0)\ (0,0)\ (0,0)\ (0,0)\ (0,0)\n(0,0)\ (0,0)\ (0,0)\ (0,0)\ (0,0)\n<0>\ <0>\ <0>\ <0>\ <0>\ <0>\ <0>\ <0>\n\"\n\ \ \ \ \ \ \ \ update\n\ \ \ \ \ \ \ \ _AS_CanvasUpdate\n\ \ \ \ \}\n\ \ \ \ \$widget(tlAdvanced)\ configure\ -cursor\ \"\"\n\ \ \ \ set\ _Adv_JustClear\ 1\n\} \
        -padx 3m -text Clear 
    button $base.fra21.fra31.fra42.but77 \
        \
        -command {if { ![WinExecCommand nowait $_25PCAL_NAME $V_CurInterface] } {
    set _Adv_JustClear 0
    update
    after 300
    unset V_PanelInfo($V_CurInterface,2)
    _AS_CanvasUpdate
}} \
        -padx 3m -text Calibrate 
    button $base.fra21.fra31.fra42.but73 \
        -text On 
    button $base.fra21.fra31.fra42.but74 \
        -text Save 
    button $base.fra21.fra31.fra42.but75 \
        -text Load 
    frame $base.fra21.fra31.fra72 \
        -borderwidth 2 -height 4 -width 125 
    frame $base.fra21.fra19 \
        -borderwidth 2 -height 75 -relief raised -width 125 
    frame $base.fra21.fra19.fra23 \
        -borderwidth 2 -height 75 -width 125 
    frame $base.fra21.fra19.fra23.fra33 \
        -borderwidth 2 -height 75 -relief groove -width 125 
    frame $base.fra21.fra19.fra23.fra33.fra28 \
        -borderwidth 2 -height 75 -width 125 
    frame $base.fra21.fra19.fra23.fra33.fra28.fra56 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra21.fra19.fra23.fra33.fra28.fra56.lab59 \
        -anchor e -text {A } -width 3 
    scale $base.fra21.fra19.fra23.fra33.fra28.fra56.sca60 \
        -bigincrement 0.0 -command {_AS_ScaleChange A} -from 0.0 \
        -orient horizontal -resolution 1.0 -showvalue 0 -sliderlength 10 \
        -tickinterval 0.0 -to 2047.0 -variable V_AccLimit -width 10 
    frame $base.fra21.fra19.fra23.fra33.fra28.fra56.fra61 \
        -borderwidth 2 -height 24 -width 1 
    frame $base.fra21.fra19.fra23.fra33.fra28.fra65 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra21.fra19.fra23.fra33.fra28.fra65.lab59 \
        -anchor e -text {B } -width 3 
    scale $base.fra21.fra19.fra23.fra33.fra28.fra65.sca60 \
        -bigincrement 0.0 -command {_AS_ScaleChange B} -from 0.0 \
        -orient horizontal -resolution 1.0 -showvalue 0 -sliderlength 10 \
        -tickinterval 0.0 -to 1024.0 -variable V_ADLimit -width 10 
    frame $base.fra21.fra19.fra23.fra33.fra28.fra65.fra61 \
        -borderwidth 2 -height 24 -width 1 
    frame $base.fra21.fra19.fra23.fra33.fra28.fra66 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra21.fra19.fra23.fra33.fra28.fra66.lab59 \
        -anchor e -text {C } -width 3 
    scale $base.fra21.fra19.fra23.fra33.fra28.fra66.sca60 \
        -bigincrement 0.0 -command {_AS_ScaleChange C} -from 3.0 \
        -orient horizontal -resolution 1.0 -showvalue 0 -sliderlength 10 \
        -tickinterval 0.0 -to 128.0 -variable V_Debounced -width 10 
    frame $base.fra21.fra19.fra23.fra33.fra28.fra66.fra61 \
        -borderwidth 2 -height 24 -width 1 
    frame $base.fra21.fra19.fra23.fra33.fra29 \
        -borderwidth 2 -height 75 -width 125 
    frame $base.fra21.fra19.fra23.fra33.fra29.fra24 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra21.fra19.fra23.fra33.fra29.fra24.lab59 \
        -anchor e -text {D } -width 4 
    scale $base.fra21.fra19.fra23.fra33.fra29.fra24.sca60 \
        -bigincrement 0.0 -command {_AS_ScaleChange D} -from 0.0 \
        -orient horizontal -resolution 1.0 -showvalue 0 -sliderlength 10 \
        -tickinterval 0.0 -to 255.0 -variable V_Fast -width 10 
    frame $base.fra21.fra19.fra23.fra33.fra29.fra24.fra61 \
        -borderwidth 2 -height 24 -width 2 
    frame $base.fra21.fra19.fra23.fra33.fra29.fra25 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra21.fra19.fra23.fra33.fra29.fra25.lab59 \
        -anchor e -text {E } -width 4 
    scale $base.fra21.fra19.fra23.fra33.fra29.fra25.sca60 \
        -bigincrement 0.0 -command {_AS_ScaleChange E} -from 0.0 \
        -orient horizontal -resolution 1.0 -showvalue 0 -sliderlength 10 \
        -tickinterval 0.0 -to 255.0 -variable V_Slow -width 10 
    frame $base.fra21.fra19.fra23.fra33.fra29.fra25.fra61 \
        -borderwidth 2 -height 24 -width 2 
    frame $base.fra21.fra19.fra23.fra33.fra29.fra40 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra21.fra19.fra23.fra33.fra29.fra40.lab59 \
        -anchor e -width 4 
    frame $base.fra21.fra19.fra23.fra33.fra29.fra40.fra61 \
        -borderwidth 2 -height 24 -width 2 
    frame $base.fra21.fra19.fra23.fra34 \
        -borderwidth 2 -height 75 -width 125 
    frame $base.fra21.fra19.fra23.fra34.fra25 \
        -borderwidth 2 -height 75 -relief groove -width 125 
    frame $base.fra21.fra19.fra23.fra34.fra25.fra39 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra21.fra19.fra23.fra34.fra25.fra39.lab59 \
        -anchor e -text Switch 
    checkbutton $base.fra21.fra19.fra23.fra34.fra25.che37 \
        -anchor center -command {_AS_ScaleChange filter $V_FilterOn x} \
        -text {Filter                } -variable V_FilterOn 
    checkbutton $base.fra21.fra19.fra23.fra34.fra25.che38 \
        -anchor center -command {_AS_ScaleChange power $V_PowerSaveOn x} \
        -text {Power Saving   } -variable V_PowerSaveOn 
    frame $base.fra21.fra19.fra23.fra34.fra26 \
        -borderwidth 2 -height 75 -relief groove -width 125 
    frame $base.fra21.fra19.fra23.fra34.fra26.fra56 \
        -borderwidth 2 -width 125 
    label $base.fra21.fra19.fra23.fra34.fra26.fra56.lab59 \
        -anchor e -text Lift-Off 
    frame $base.fra21.fra19.fra23.fra34.fra26.fra65 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra21.fra19.fra23.fra34.fra26.fra65.lab59 \
        -anchor e -text {Count } -width 7 
    scale $base.fra21.fra19.fra23.fra34.fra26.fra65.sca60 \
        -bigincrement 0.0 -command {_AS_ScaleChange Count} -from 0.0 \
        -orient horizontal -resolution 1.0 -showvalue 0 -sliderlength 10 \
        -tickinterval 0.0 -to 80.0 -variable V_LiftCount -width 10 
    frame $base.fra21.fra19.fra23.fra34.fra26.fra65.fra61 \
        -borderwidth 2 -height 24 -width 7 
    frame $base.fra21.fra19.fra23.fra34.fra26.fra66 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra21.fra19.fra23.fra34.fra26.fra66.lab59 \
        -anchor e -text {Range } -width 7 
    scale $base.fra21.fra19.fra23.fra34.fra26.fra66.sca60 \
        -bigincrement 0.0 -command {_AS_ScaleChange Range} -from 3.0 \
        -orient horizontal -resolution 1.0 -showvalue 0 -sliderlength 10 \
        -tickinterval 0.0 -to 50.0 -variable V_LiftRange -width 10 
    frame $base.fra21.fra19.fra23.fra34.fra26.fra66.fra61 \
        -borderwidth 2 -height 24 -width 7 
    frame $base.fra21.fra19.fra23.fra34.fra26.fra18 \
        -borderwidth 2 -height 1 -width 125 
    frame $base.fra21.fra19.fra24 \
        -borderwidth 2 -height 75 -width 125 
    button $base.fra21.fra19.fra24.but23 \
        \
        -command {global g_ParmList
set list $g_ParmList
set change 0
foreach var $list {
    set dat [subst \$EPR_Default$var]
    if { [subst \$V_$var] != $dat } {
        incr change
    }
    set V_$var $dat
}
if { $change } {
    $widget(btAdvancedApply) configure -state normal
}} \
        -padx 3m -text Default 
    button $base.fra21.fra19.fra24.but24 \
        -command _AS_ApplyPressed -state disabled -text Apply 
    frame $base.fra21.fra19.fra25 \
        -borderwidth 2 -height 4 -width 125 
    frame $base.fra49 \
        -borderwidth 2 -height 91 -width 161 
    button $base.fra49.but25 \
        -command PopMainWindow -height 1 -padx 3m -text Close 
    frame $base.fra51 \
        -borderwidth 2 -height 6 -width 161 
    ###################
    # SETTING GEOMETRY
    ###################
    pack $base.fra21 \
        -in $base -anchor center -expand 1 -fill both -padx 6 -pady 6 \
        -side top 
    grid columnconf $base.fra21 1 -weight 1
    grid rowconf $base.fra21 1 -weight 1
    grid $base.fra21.lab80 \
        -in $base.fra21 -column 0 -row 0 -columnspan 1 -rowspan 1 -ipadx 3 \
        -ipady 2 
    grid $base.fra21.lab82 \
        -in $base.fra21 -column 1 -row 0 -columnspan 1 -rowspan 1 -ipadx 3 \
        -ipady 2 -padx 1 -sticky w 
    grid $base.fra21.fra31 \
        -in $base.fra21 -column 0 -row 1 -columnspan 10 -rowspan 10 \
        -sticky nesw 
    grid columnconf $base.fra21.fra31 0 -weight 1
    grid rowconf $base.fra21.fra31 0 -weight 40
    grid rowconf $base.fra21.fra31 10 -weight 1
    grid $base.fra21.fra31.can36 \
        -in $base.fra21.fra31 -column 0 -row 0 -columnspan 1 -rowspan 1 \
        -padx 6 -pady 6 
    grid $base.fra21.fra31.fra42 \
        -in $base.fra21.fra31 -column 0 -row 1 -columnspan 10 -rowspan 1 \
        -padx 3 -sticky ew 
    pack $base.fra21.fra31.fra42.but76 \
        -in $base.fra21.fra31.fra42 -anchor e -expand 1 -fill none -ipady 3 \
        -padx 2 -side left 
    pack $base.fra21.fra31.fra42.but77 \
        -in $base.fra21.fra31.fra42 -anchor e -expand 0 -fill none -ipady 3 \
        -padx 2 -side left 
    grid $base.fra21.fra31.fra72 \
        -in $base.fra21.fra31 -column 0 -row 2 -columnspan 1 -rowspan 1 
    grid $base.fra21.fra19 \
        -in $base.fra21 -column 0 -row 1 -columnspan 10 -rowspan 10 \
        -sticky nesw 
    grid columnconf $base.fra21.fra19 0 -weight 1
    grid rowconf $base.fra21.fra19 0 -weight 1
    grid $base.fra21.fra19.fra23 \
        -in $base.fra21.fra19 -column 0 -row 0 -columnspan 1 -rowspan 1 \
        -padx 1 -pady 3 -sticky nesw 
    pack $base.fra21.fra19.fra23.fra33 \
        -in $base.fra21.fra19.fra23 -anchor center -expand 1 -fill both \
        -padx 3 -side top 
    grid columnconf $base.fra21.fra19.fra23.fra33 0 -weight 1
    grid columnconf $base.fra21.fra19.fra23.fra33 1 -weight 1
    grid rowconf $base.fra21.fra19.fra23.fra33 0 -weight 1
    grid $base.fra21.fra19.fra23.fra33.fra28 \
        -in $base.fra21.fra19.fra23.fra33 -column 0 -row 0 -columnspan 1 \
        -rowspan 1 -sticky nesw 
    pack $base.fra21.fra19.fra23.fra33.fra28.fra56 \
        -in $base.fra21.fra19.fra23.fra33.fra28 -anchor center -expand 1 \
        -fill x -side top 
    grid columnconf $base.fra21.fra19.fra23.fra33.fra28.fra56 2 -weight 1
    grid $base.fra21.fra19.fra23.fra33.fra28.fra56.lab59 \
        -in $base.fra21.fra19.fra23.fra33.fra28.fra56 -column 1 -row 0 \
        -columnspan 1 -rowspan 1 -sticky e 
    grid $base.fra21.fra19.fra23.fra33.fra28.fra56.sca60 \
        -in $base.fra21.fra19.fra23.fra33.fra28.fra56 -column 2 -row 0 \
        -columnspan 1 -rowspan 1 -sticky ew 
    grid $base.fra21.fra19.fra23.fra33.fra28.fra56.fra61 \
        -in $base.fra21.fra19.fra23.fra33.fra28.fra56 -column 3 -row 0 \
        -columnspan 1 -rowspan 1 
    pack $base.fra21.fra19.fra23.fra33.fra28.fra65 \
        -in $base.fra21.fra19.fra23.fra33.fra28 -anchor center -expand 1 \
        -fill x -side top 
    grid columnconf $base.fra21.fra19.fra23.fra33.fra28.fra65 2 -weight 1
    grid $base.fra21.fra19.fra23.fra33.fra28.fra65.lab59 \
        -in $base.fra21.fra19.fra23.fra33.fra28.fra65 -column 1 -row 0 \
        -columnspan 1 -rowspan 1 -sticky e 
    grid $base.fra21.fra19.fra23.fra33.fra28.fra65.sca60 \
        -in $base.fra21.fra19.fra23.fra33.fra28.fra65 -column 2 -row 0 \
        -columnspan 1 -rowspan 1 -sticky ew 
    grid $base.fra21.fra19.fra23.fra33.fra28.fra65.fra61 \
        -in $base.fra21.fra19.fra23.fra33.fra28.fra65 -column 3 -row 0 \
        -columnspan 1 -rowspan 1 
    pack $base.fra21.fra19.fra23.fra33.fra28.fra66 \
        -in $base.fra21.fra19.fra23.fra33.fra28 -anchor center -expand 1 \
        -fill x -side top 
    grid columnconf $base.fra21.fra19.fra23.fra33.fra28.fra66 2 -weight 1
    grid $base.fra21.fra19.fra23.fra33.fra28.fra66.lab59 \
        -in $base.fra21.fra19.fra23.fra33.fra28.fra66 -column 1 -row 0 \
        -columnspan 1 -rowspan 1 -sticky e 
    grid $base.fra21.fra19.fra23.fra33.fra28.fra66.sca60 \
        -in $base.fra21.fra19.fra23.fra33.fra28.fra66 -column 2 -row 0 \
        -columnspan 1 -rowspan 1 -sticky ew 
    grid $base.fra21.fra19.fra23.fra33.fra28.fra66.fra61 \
        -in $base.fra21.fra19.fra23.fra33.fra28.fra66 -column 3 -row 0 \
        -columnspan 1 -rowspan 1 
    grid $base.fra21.fra19.fra23.fra33.fra29 \
        -in $base.fra21.fra19.fra23.fra33 -column 1 -row 0 -columnspan 1 \
        -rowspan 1 -sticky nesw 
    pack $base.fra21.fra19.fra23.fra33.fra29.fra24 \
        -in $base.fra21.fra19.fra23.fra33.fra29 -anchor center -expand 1 \
        -fill x -side top 
    grid columnconf $base.fra21.fra19.fra23.fra33.fra29.fra24 2 -weight 1
    grid $base.fra21.fra19.fra23.fra33.fra29.fra24.lab59 \
        -in $base.fra21.fra19.fra23.fra33.fra29.fra24 -column 1 -row 0 \
        -columnspan 1 -rowspan 1 -sticky e 
    grid $base.fra21.fra19.fra23.fra33.fra29.fra24.sca60 \
        -in $base.fra21.fra19.fra23.fra33.fra29.fra24 -column 2 -row 0 \
        -columnspan 1 -rowspan 1 -sticky ew 
    grid $base.fra21.fra19.fra23.fra33.fra29.fra24.fra61 \
        -in $base.fra21.fra19.fra23.fra33.fra29.fra24 -column 3 -row 0 \
        -columnspan 1 -rowspan 1 
    pack $base.fra21.fra19.fra23.fra33.fra29.fra25 \
        -in $base.fra21.fra19.fra23.fra33.fra29 -anchor center -expand 1 \
        -fill x -side top 
    grid columnconf $base.fra21.fra19.fra23.fra33.fra29.fra25 2 -weight 1
    grid $base.fra21.fra19.fra23.fra33.fra29.fra25.lab59 \
        -in $base.fra21.fra19.fra23.fra33.fra29.fra25 -column 1 -row 0 \
        -columnspan 1 -rowspan 1 -sticky e 
    grid $base.fra21.fra19.fra23.fra33.fra29.fra25.sca60 \
        -in $base.fra21.fra19.fra23.fra33.fra29.fra25 -column 2 -row 0 \
        -columnspan 1 -rowspan 1 -sticky ew 
    grid $base.fra21.fra19.fra23.fra33.fra29.fra25.fra61 \
        -in $base.fra21.fra19.fra23.fra33.fra29.fra25 -column 3 -row 0 \
        -columnspan 1 -rowspan 1 
    pack $base.fra21.fra19.fra23.fra33.fra29.fra40 \
        -in $base.fra21.fra19.fra23.fra33.fra29 -anchor center -expand 1 \
        -fill x -side top 
    grid columnconf $base.fra21.fra19.fra23.fra33.fra29.fra40 2 -weight 1
    grid $base.fra21.fra19.fra23.fra33.fra29.fra40.lab59 \
        -in $base.fra21.fra19.fra23.fra33.fra29.fra40 -column 1 -row 0 \
        -columnspan 1 -rowspan 1 -sticky e 
    grid $base.fra21.fra19.fra23.fra33.fra29.fra40.fra61 \
        -in $base.fra21.fra19.fra23.fra33.fra29.fra40 -column 3 -row 0 \
        -columnspan 1 -rowspan 1 
    pack $base.fra21.fra19.fra23.fra34 \
        -in $base.fra21.fra19.fra23 -anchor center -expand 1 -fill both \
        -side top 
    grid columnconf $base.fra21.fra19.fra23.fra34 0 -weight 20
    grid columnconf $base.fra21.fra19.fra23.fra34 1 -weight 9
    grid rowconf $base.fra21.fra19.fra23.fra34 0 -weight 1
    grid $base.fra21.fra19.fra23.fra34.fra25 \
        -in $base.fra21.fra19.fra23.fra34 -column 0 -row 0 -columnspan 1 \
        -rowspan 1 -padx 1 -sticky nesw 
    pack $base.fra21.fra19.fra23.fra34.fra25.fra39 \
        -in $base.fra21.fra19.fra23.fra34.fra25 -anchor center -expand 0 \
        -fill x -side top 
    grid columnconf $base.fra21.fra19.fra23.fra34.fra25.fra39 2 -weight 1
    grid $base.fra21.fra19.fra23.fra34.fra25.fra39.lab59 \
        -in $base.fra21.fra19.fra23.fra34.fra25.fra39 -column 1 -row 0 \
        -columnspan 1 -rowspan 1 -padx 3 -sticky e 
    pack $base.fra21.fra19.fra23.fra34.fra25.che37 \
        -in $base.fra21.fra19.fra23.fra34.fra25 -anchor w -expand 0 -fill y \
        -padx 3 -side top 
    pack $base.fra21.fra19.fra23.fra34.fra25.che38 \
        -in $base.fra21.fra19.fra23.fra34.fra25 -anchor w -expand 0 -fill y \
        -padx 3 -side top 
    grid $base.fra21.fra19.fra23.fra34.fra26 \
        -in $base.fra21.fra19.fra23.fra34 -column 1 -row 0 -columnspan 1 \
        -rowspan 1 -padx 1 -sticky nesw 
    pack $base.fra21.fra19.fra23.fra34.fra26.fra56 \
        -in $base.fra21.fra19.fra23.fra34.fra26 -anchor center -expand 0 \
        -fill x -side top 
    grid columnconf $base.fra21.fra19.fra23.fra34.fra26.fra56 2 -weight 1
    grid $base.fra21.fra19.fra23.fra34.fra26.fra56.lab59 \
        -in $base.fra21.fra19.fra23.fra34.fra26.fra56 -column 1 -row 0 \
        -columnspan 1 -rowspan 1 -padx 3 -sticky e 
    pack $base.fra21.fra19.fra23.fra34.fra26.fra65 \
        -in $base.fra21.fra19.fra23.fra34.fra26 -anchor nw -expand 0 \
        -fill both -side top 
    grid columnconf $base.fra21.fra19.fra23.fra34.fra26.fra65 2 -weight 1
    grid $base.fra21.fra19.fra23.fra34.fra26.fra65.lab59 \
        -in $base.fra21.fra19.fra23.fra34.fra26.fra65 -column 1 -row 0 \
        -columnspan 1 -rowspan 1 -sticky e 
    grid $base.fra21.fra19.fra23.fra34.fra26.fra65.sca60 \
        -in $base.fra21.fra19.fra23.fra34.fra26.fra65 -column 2 -row 0 \
        -columnspan 1 -rowspan 1 -sticky ew 
    grid $base.fra21.fra19.fra23.fra34.fra26.fra65.fra61 \
        -in $base.fra21.fra19.fra23.fra34.fra26.fra65 -column 3 -row 0 \
        -columnspan 1 -rowspan 1 
    pack $base.fra21.fra19.fra23.fra34.fra26.fra66 \
        -in $base.fra21.fra19.fra23.fra34.fra26 -anchor nw -expand 0 \
        -fill both -side top 
    grid columnconf $base.fra21.fra19.fra23.fra34.fra26.fra66 2 -weight 1
    grid $base.fra21.fra19.fra23.fra34.fra26.fra66.lab59 \
        -in $base.fra21.fra19.fra23.fra34.fra26.fra66 -column 1 -row 0 \
        -columnspan 1 -rowspan 1 -sticky e 
    grid $base.fra21.fra19.fra23.fra34.fra26.fra66.sca60 \
        -in $base.fra21.fra19.fra23.fra34.fra26.fra66 -column 2 -row 0 \
        -columnspan 1 -rowspan 1 -sticky ew 
    grid $base.fra21.fra19.fra23.fra34.fra26.fra66.fra61 \
        -in $base.fra21.fra19.fra23.fra34.fra26.fra66 -column 3 -row 0 \
        -columnspan 1 -rowspan 1 
    pack $base.fra21.fra19.fra23.fra34.fra26.fra18 \
        -in $base.fra21.fra19.fra23.fra34.fra26 -anchor center -expand 1 \
        -fill both -side top 
    grid columnconf $base.fra21.fra19.fra23.fra34.fra26.fra18 2 -weight 1
    grid $base.fra21.fra19.fra24 \
        -in $base.fra21.fra19 -column 0 -row 1 -columnspan 1 -rowspan 1 \
        -padx 3 -sticky ew 
    pack $base.fra21.fra19.fra24.but23 \
        -in $base.fra21.fra19.fra24 -anchor e -expand 1 -fill none -ipady 3 \
        -padx 3 -side left 
    pack $base.fra21.fra19.fra24.but24 \
        -in $base.fra21.fra19.fra24 -anchor e -expand 0 -fill none -ipady 3 \
        -padx 2 -side left 
    grid $base.fra21.fra19.fra25 \
        -in $base.fra21.fra19 -column 0 -row 2 -columnspan 1 -rowspan 1 
    pack $base.fra49 \
        -in $base -anchor center -expand 0 -fill x -padx 2 -side top 
    pack $base.fra49.but25 \
        -in $base.fra49 -anchor ne -expand 1 -fill none -ipadx 20 -ipady 7 \
        -padx 3 -side left 
    pack $base.fra51 \
        -in $base -anchor center -expand 0 -fill none -padx 4 -side bottom 

    vTcl:FireEvent $base <<Ready>>
}

proc vTclWindow.top19 {base {container 0}} {
    if {$base == ""} {
        set base .top19
    }
    if {[winfo exists $base] && (!$container)} {
        wm deiconify $base; return
    }

    global widget
    vTcl:DefineAlias "$base" "tlExecCommand" vTcl:Toplevel:WidgetProc "" 1
    vTcl:DefineAlias "$base.fra33" "Frame69" vTcl:WidgetProc "tlExecCommand" 1
    vTcl:DefineAlias "$base.fra33.but38" "btExecCommandOK" vTcl:WidgetProc "tlExecCommand" 1
    vTcl:DefineAlias "$base.fra33.fra35" "Frame68" vTcl:WidgetProc "tlExecCommand" 1
    vTcl:DefineAlias "$base.fra33.fra35.01" "Scrollbar9" vTcl:WidgetProc "tlExecCommand" 1
    vTcl:DefineAlias "$base.fra33.fra35.02" "Scrollbar10" vTcl:WidgetProc "tlExecCommand" 1
    vTcl:DefineAlias "$base.fra33.fra35.03" "ttExecCommand" vTcl:WidgetProc "tlExecCommand" 1
    vTcl:DefineAlias "$base.fra33.fra36" "Frame70" vTcl:WidgetProc "tlExecCommand" 1

    ###################
    # CREATING WIDGETS
    ###################
    if {!$container} {
    vTcl:toplevel $base -class Toplevel
    wm withdraw $base
    wm focusmodel $base passive
    wm geometry $base 314x268+84+128; update
    wm maxsize $base 1009 738
    wm minsize $base 1 1
    wm overrideredirect $base 0
    wm resizable $base 0 0
    wm title $base "Exec Command"
    vTcl:FireEvent $base <<Create>>
    }
    frame $base.fra33 \
        -borderwidth 2 -height 75 -width 125 
    frame $base.fra33.fra35 \
        -borderwidth 1 -height 30 -relief raised -width 30 
    scrollbar $base.fra33.fra35.01 \
        -command "$base.fra33.fra35.03 xview" -orient horizontal 
    scrollbar $base.fra33.fra35.02 \
        -command "$base.fra33.fra35.03 yview" 
    text $base.fra33.fra35.03 \
        -cursor xterm \
        -font -Adobe-Helvetica-Medium-R-Normal-*-*-120-*-*-*-*-*-* -height 10 \
        -width 20 -wrap none -xscrollcommand "$base.fra33.fra35.01 set" \
        -yscrollcommand "$base.fra33.fra35.02 set" 
    frame $base.fra33.fra36 \
        -borderwidth 2 -height 4 -width 115 
    button $base.fra33.but38 \
        -command PopMainWindow -text OK 
    ###################
    # SETTING GEOMETRY
    ###################
    grid columnconf $base 0 -weight 1
    grid rowconf $base 1 -weight 1
    pack $base.fra33 \
        -in $base -anchor center -expand 1 -fill both -padx 6 -pady 6 \
        -side top 
    grid columnconf $base.fra33 0 -weight 1
    grid rowconf $base.fra33 1 -weight 4
    grid rowconf $base.fra33 3 -weight 1
    grid $base.fra33.fra35 \
        -in $base.fra33 -column 0 -row 1 -columnspan 1 -rowspan 1 \
        -sticky nesw 
    grid columnconf $base.fra33.fra35 0 -weight 1
    grid rowconf $base.fra33.fra35 0 -weight 1
    grid $base.fra33.fra35.01 \
        -in $base.fra33.fra35 -column 0 -row 1 -columnspan 1 -rowspan 1 \
        -sticky ew 
    grid $base.fra33.fra35.02 \
        -in $base.fra33.fra35 -column 1 -row 0 -columnspan 1 -rowspan 1 \
        -sticky ns 
    grid $base.fra33.fra35.03 \
        -in $base.fra33.fra35 -column 0 -row 0 -columnspan 1 -rowspan 1 \
        -sticky nesw 
    grid $base.fra33.fra36 \
        -in $base.fra33 -column 0 -row 2 -columnspan 1 -rowspan 1 
    grid $base.fra33.but38 \
        -in $base.fra33 -column 0 -row 3 -columnspan 1 -rowspan 1 -ipadx 23 \
        -ipady 2 -pady 5 -sticky ns 

    vTcl:FireEvent $base <<Ready>>
}

proc vTclWindow.top41 {base {container 0}} {
    if {$base == ""} {
        set base .top41
    }
    if {[winfo exists $base] && (!$container)} {
        wm deiconify $base; return
    }

    global widget
    vTcl:DefineAlias "$base" "tlAbout" vTcl:Toplevel:WidgetProc "" 1
    vTcl:DefineAlias "$base.but26" "btAboutOK" vTcl:WidgetProc "tlAbout" 1
    vTcl:DefineAlias "$base.fra24" "Frame71" vTcl:WidgetProc "tlAbout" 1
    vTcl:DefineAlias "$base.fra24.fra37" "Frame73" vTcl:WidgetProc "tlAbout" 1
    vTcl:DefineAlias "$base.fra24.lab35" "lbAboutLogo" vTcl:WidgetProc "tlAbout" 1
    vTcl:DefineAlias "$base.fra24.tex36" "ttAboutCRight" vTcl:WidgetProc "tlAbout" 1
    vTcl:DefineAlias "$base.fra25" "Frame64" vTcl:WidgetProc "tlAbout" 1
    vTcl:DefineAlias "$base.fra25.01" "Scrollbar5" vTcl:WidgetProc "tlAbout" 1
    vTcl:DefineAlias "$base.fra25.02" "Scrollbar6" vTcl:WidgetProc "tlAbout" 1
    vTcl:DefineAlias "$base.fra25.03" "ttAboutInfo" vTcl:WidgetProc "tlAbout" 1

    ###################
    # CREATING WIDGETS
    ###################
    if {!$container} {
    vTcl:toplevel $base -class Toplevel
    wm withdraw $base
    wm focusmodel $base passive
    wm geometry $base 544x412+49+32; update
    wm maxsize $base 1009 738
    wm minsize $base 1 1
    wm overrideredirect $base 0
    wm resizable $base 0 0
    wm title $base "About"
    vTcl:FireEvent $base <<Create>>
    }
    frame $base.fra24 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra24.lab35 \
        -padx 1 -pady 1 -text image 
    text $base.fra24.tex36 \
        -height 6 -padx 10 -relief flat -width 15 
    frame $base.fra24.fra37 \
        -borderwidth 2 -height 75 -width 9 
    frame $base.fra25 \
        -borderwidth 1 -height 30 -relief raised -width 30 
    scrollbar $base.fra25.01 \
        -command "$base.fra25.03 xview" -orient horizontal 
    scrollbar $base.fra25.02 \
        -command "$base.fra25.03 yview" 
    text $base.fra25.03 \
        -font -Adobe-Helvetica-Medium-R-Normal-*-*-120-*-*-*-*-*-* -height 1 \
        -padx 1 -pady 1 -width 1 -xscrollcommand "$base.fra25.01 set" \
        -yscrollcommand "$base.fra25.02 set" 
    button $base.but26 \
        -command PopMainWindow -text OK -width 10 
    ###################
    # SETTING GEOMETRY
    ###################
    grid columnconf $base 0 -weight 1
    grid columnconf $base 1 -weight 10
    grid rowconf $base 3 -weight 1
    grid $base.fra24 \
        -in $base -column 0 -row 0 -columnspan 10 -rowspan 1 -padx 6 -pady 8 \
        -sticky nesw 
    grid columnconf $base.fra24 2 -weight 1
    grid $base.fra24.lab35 \
        -in $base.fra24 -column 0 -row 0 -columnspan 1 -rowspan 1 
    grid $base.fra24.tex36 \
        -in $base.fra24 -column 2 -row 0 -columnspan 1 -rowspan 1 \
        -sticky nesw 
    grid $base.fra24.fra37 \
        -in $base.fra24 -column 1 -row 0 -columnspan 1 -rowspan 1 
    grid $base.fra25 \
        -in $base -column 0 -row 1 -columnspan 10 -rowspan 10 -padx 8 \
        -sticky nesw 
    grid columnconf $base.fra25 0 -weight 1
    grid rowconf $base.fra25 0 -weight 2
    grid $base.fra25.01 \
        -in $base.fra25 -column 0 -row 1 -columnspan 1 -rowspan 1 -sticky ew 
    grid $base.fra25.02 \
        -in $base.fra25 -column 1 -row 0 -columnspan 1 -rowspan 1 -sticky ns 
    grid $base.fra25.03 \
        -in $base.fra25 -column 0 -row 0 -columnspan 1 -rowspan 1 \
        -sticky nesw 
    grid $base.but26 \
        -in $base -column 1 -row 11 -columnspan 1 -rowspan 1 -ipady 6 -padx 9 \
        -pady 9 -sticky e 

    vTcl:FireEvent $base <<Ready>>
}

proc vTclWindow.top52 {base {container 0}} {
    if {$base == ""} {
        set base .top52
    }
    if {[winfo exists $base] && (!$container)} {
        wm deiconify $base; return
    }

    global widget
    vTcl:DefineAlias "$base" "tlTesting" vTcl:Toplevel:WidgetProc "" 1

    ###################
    # CREATING WIDGETS
    ###################
    if {!$container} {
    vTcl:toplevel $base -class Toplevel
    wm withdraw $base
    wm focusmodel $base passive
    wm geometry $base 1x1+0+0; update
    wm maxsize $base 1 1
    wm minsize $base 1 1
    wm overrideredirect $base 0
    wm resizable $base 0 0
    wm title $base "Testing..."
    vTcl:FireEvent $base <<Create>>
    }
    ###################
    # SETTING GEOMETRY
    ###################

    vTcl:FireEvent $base <<Ready>>
}

proc vTclWindow.top63 {base {container 0}} {
    if {$base == ""} {
        set base .top63
    }
    if {[winfo exists $base] && (!$container)} {
        wm deiconify $base; return
    }

    global widget
    vTcl:DefineAlias "$base" "tlMain" vTcl:Toplevel:WidgetProc "" 1
    vTcl:DefineAlias "$base.fra18" "Frame17" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra18.fra67" "Frame19" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra18.fra67.lab164" "lbSound" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra18.fra67.rad72" "rbNoSound" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra18.fra67.rad73" "rbTouchDown" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra18.fra67.rad74" "rbLiftUp" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra18.lab69" "lbLogo" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19" "Frame18" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18" "Frame102" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra19" "Frame103" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra19.che22" "cbModeButton" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra19.rad20" "rbDrawingMode" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra19.rad21" "rbDesktopMode" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra30" "Frame104" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra30.fra28" "Frame20" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra30.fra28.but78" "btDblClickSpeedSlow" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra30.fra28.but81" "btDblClickSpeedFast" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra30.fra28.sca80" "scDblClickSpeed" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra30.fra29" "Frame35" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra30.fra29.but78" "btDblClickAreaSmall" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra30.fra29.but81" "btDblClickAreaLarge" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra30.fra29.sca80" "scDblClickArea" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra30.lab26" "lbDblClickSpeed" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.fra30.lab27" "lbDblClickArea" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra18.lab164" "lbMouseMode" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra19" "Frame107" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra19.but20" "btDrawTest" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra19.but21" "bt4PCal" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra19.but24" "btAdvanced" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra19.but25" "btAbout" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra19.fra19.but26" "btClose" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73" "Frame8" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23" "Frame40" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23.fra54" "Frame44" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23.fra59" "Frame47" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23.lab24" "lbInterface" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23.lab40" "lbType" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23.lab41" "lbVersion" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23.lab42" "lbStatus" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23.lis57" "lbPanelListInterface" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23.lis58" "lbPanelListType" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23.lis59" "lbPanelListVersion" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23.lis60" "lbPanelListStatus" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra23.scr25" "sbPanelList" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra64" "Frame24" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra64.but104" "btPanelListAdd" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra64.but105" "btPanelListRemove" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.fra64.but106" "btPanelListRescan" vTcl:WidgetProc "tlMain" 1
    vTcl:DefineAlias "$base.fra73.lab165" "lbPanelCandidate" vTcl:WidgetProc "tlMain" 1

    ###################
    # CREATING WIDGETS
    ###################
    if {!$container} {
    vTcl:toplevel $base -class Toplevel
    wm withdraw $base
    wm focusmodel $base passive
    wm geometry $base 617x429+17+23; update
    wm maxsize $base 1009 738
    wm minsize $base 1 1
    wm overrideredirect $base 0
    wm resizable $base 0 0
    wm title $base "main"
    vTcl:FireEvent $base <<Create>>
    }
    frame $base.fra73 \
        -borderwidth 2 -height 79 -relief groove -width 125 
    label $base.fra73.lab165 \
        -background #d0d0d0 -padx 1 -pady 1 -relief sunken \
        -text {Panel Candidate List} 
    frame $base.fra73.fra23 \
        -borderwidth 2 -height 72 -width 113 
    label $base.fra73.fra23.lab24 \
        -anchor w -pady 1 -relief groove -text Interface 
    label $base.fra73.fra23.lab40 \
        -anchor w -pady 1 -relief groove -text Type 
    label $base.fra73.fra23.lab41 \
        -anchor w -pady 1 -relief groove -text Version 
    label $base.fra73.fra23.lab42 \
        -anchor w -pady 1 -relief groove -text Status 
    frame $base.fra73.fra23.fra59 \
        -borderwidth 2 -height 8 -width 5 
    frame $base.fra73.fra23.fra54 \
        -background white -borderwidth 2 -height 100 -relief sunken \
        -width 478 
    listbox $base.fra73.fra23.lis57 \
        -background white -borderwidth 0 -height 1 -relief flat \
        -selectbackground #C3C3C3 -selectborderwidth 0 -width 0 \
        -yscrollcommand {_YScrollCmd I} 
    bindtags $base.fra73.fra23.lis57 "$base.fra73.fra23.lis57 lbPanelList Listbox $base all"
    bind $base.fra73.fra23.lis57 <FocusIn> {
        catch {focus $widget(sbPanelList)}
    }
    listbox $base.fra73.fra23.lis58 \
        -background white -borderwidth 0 -height 1 -relief flat \
        -selectbackground #C3C3C3 -selectborderwidth 0 -width 0 \
        -yscrollcommand {_YScrollCmd T} 
    bindtags $base.fra73.fra23.lis58 "$base.fra73.fra23.lis58 lbPanelList Listbox $base all"
    listbox $base.fra73.fra23.lis59 \
        -background white -borderwidth 0 -height 1 -relief flat \
        -selectbackground #C3C3C3 -selectborderwidth 0 -width 0 \
        -yscrollcommand {_YScrollCmd V} 
    bindtags $base.fra73.fra23.lis59 "$base.fra73.fra23.lis59 lbPanelList Listbox $base all"
    listbox $base.fra73.fra23.lis60 \
        -background white -borderwidth 0 -height 1 -relief flat \
        -selectbackground #C3C3C3 -selectborderwidth 0 -width 0 \
        -yscrollcommand {_YScrollCmd S} 
    bindtags $base.fra73.fra23.lis60 "$base.fra73.fra23.lis60 lbPanelList Listbox $base all"
    bind $base.fra73.fra23.lis60 <FocusIn> {
        if { $V_btAdvancedOn } {
    catch {focus $widget(btAdvanced)}
} else {
    catch {focus $widget(btClose)}
}
    }
    scrollbar $base.fra73.fra23.scr25 \
        -command _ScrollPanelList 
    frame $base.fra73.fra64 \
        -borderwidth 2 -height 75 -width 125 
    button $base.fra73.fra64.but104 \
        -anchor center \
        -command {PushMainWindow $widget(tlAddPanel) InitAddPanel} -pady 1m \
        -text Add -width 7 
    button $base.fra73.fra64.but105 \
        -command {PanelList remove} -padx 3m -pady 1m -text Remove -width 7 
    button $base.fra73.fra64.but106 \
        \
        -command {PushMainWindow $widget(tlTesting)
$widget(tlMain) configure -cursor watch
    PanelList rescan
$widget(tlMain) configure -cursor ""
PopMainWindow} \
        -padx 3m -pady 1m -text Rescan -width 7 
    frame $base.fra18 \
        -borderwidth 2 -height 75 -width 8 
    frame $base.fra18.fra67 \
        -borderwidth 2 -height 75 -relief groove -width 136 
    label $base.fra18.fra67.lab164 \
        -background #d0d0d0 -padx 1 -pady 1 -relief sunken -text Sound 
    radiobutton $base.fra18.fra67.rad72 \
        -anchor w -command _SoundModeChange -padx 1 -pady 1 -text {No Sound} \
        -variable V_SoundMode 
    radiobutton $base.fra18.fra67.rad73 \
        -anchor w -command _SoundModeChange -padx 1 -pady 1 \
        -text {Touch Down} -variable V_SoundMode 
    radiobutton $base.fra18.fra67.rad74 \
        -anchor w -command _SoundModeChange -text {Lift Up} \
        -variable V_SoundMode 
    label $base.fra18.lab69 \
        -padx 1 -pady 1 -text image 
    frame $base.fra19 \
        -borderwidth 2 -height 98 -width 125 
    frame $base.fra19.fra18 \
        -borderwidth 2 -height 75 -relief groove -width 125 
    label $base.fra19.fra18.lab164 \
        -background #d0d0d0 -padx 1 -pady 1 -relief sunken -text {Mouse Mode} 
    frame $base.fra19.fra18.fra19 \
        -borderwidth 2 -height 75 -width 125 
    radiobutton $base.fra19.fra18.fra19.rad20 \
        -anchor w -command _MouseModeChange -padx 1 -pady 1 \
        -text {Drawing Mode} -variable V_MouseMode 
    radiobutton $base.fra19.fra18.fra19.rad21 \
        -anchor w -command _MouseModeChange -padx 1 -text {Desktop Mode} \
        -variable V_MouseMode 
    checkbutton $base.fra19.fra18.fra19.che22 \
        -command _RToolChange -padx 1 -pady 1 -relief raised \
        -text {Mode Button} -variable V_RClickTool 
    frame $base.fra19.fra18.fra30 \
        -borderwidth 2 -height 75 -relief groove -width 125 
    label $base.fra19.fra18.fra30.lab26 \
        -anchor e -text {Dbl-Click Speed:} 
    label $base.fra19.fra18.fra30.lab27 \
        -anchor e -text {Dbl-Click Area:} 
    frame $base.fra19.fra18.fra30.fra28 \
        -borderwidth 2 -height 75 -width 125 
    button $base.fra19.fra18.fra30.fra28.but78 \
        \
        -command {set old_dcs $V_DblClickSpeed
incr V_DblClickSpeed
if { $old_dcs != $V_DblClickSpeed } {
    UpdateDriverMode $_DblClickSpeed $V_DblClickSpeed
}} \
        -padx 3m -text {Slow <<} -width 7 
    scale $base.fra19.fra18.fra30.fra28.sca80 \
        -bigincrement 0.0 -command {catch {UpdateDriverMode $_DblClickSpeed}} \
        -from 100.0 -orient horizontal -resolution 1.0 -showvalue 0 \
        -sliderlength 10 -to 0.0 -variable V_DblClickSpeed -width 12 
    button $base.fra19.fra18.fra30.fra28.but81 \
        \
        -command {set old_dcs $V_DblClickSpeed
incr V_DblClickSpeed -1
if { $old_dcs != $V_DblClickSpeed } {
    UpdateDriverMode $_DblClickSpeed $V_DblClickSpeed
}} \
        -padx 3m -pady 1m -text {>> Fast} -width 7 
    frame $base.fra19.fra18.fra30.fra29 \
        -borderwidth 2 -height 75 -width 125 
    button $base.fra19.fra18.fra30.fra29.but78 \
        \
        -command {set old_dca $V_DblClickArea
incr V_DblClickArea -1
if { $old_dca != $V_DblClickArea } {
    UpdateDriverMode $_DblClickArea $V_DblClickArea
}} \
        -padx 3m -text {Small <<} -width 7 
    scale $base.fra19.fra18.fra30.fra29.sca80 \
        -bigincrement 0.0 -command {catch {UpdateDriverMode $_DblClickArea}} \
        -from 0.0 -orient horizontal -resolution 1.0 -showvalue 0 \
        -sliderlength 10 -tickinterval 0.0 -to 100.0 -variable V_DblClickArea \
        -width 12 
    button $base.fra19.fra18.fra30.fra29.but81 \
        \
        -command {set old_dca $V_DblClickArea
incr V_DblClickArea
if { $old_dca != $V_DblClickArea } {
    UpdateDriverMode $_DblClickArea $V_DblClickArea
}} \
        -height 1 -padx 3m -text {>> Large} -width 7 
    frame $base.fra19.fra19 \
        -borderwidth 2 -height 75 -width 124 
    button $base.fra19.fra19.but20 \
        -command {WinExecCommand nowait $DRAW_NAME $V_CurInterface} \
        -text {Draw Test} 
    button $base.fra19.fra19.but21 \
        -command {WinExecCommand nowait $_4PCAL_NAME $V_CurInterface} \
        -padx 3m -pady 1m -text {4-Pt Cal} 
    button $base.fra19.fra19.but24 \
        -command {PushMainWindow $widget(tlAdvanced) InitAdvanced} -padx 3m \
        -text {Advanced >>} 
    button $base.fra19.fra19.but25 \
        -command {PushMainWindow $widget(tlAbout) InitAbout} -padx 3m \
        -text About 
    button $base.fra19.fra19.but26 \
        -command PopMainWindow -text Close 
    ###################
    # SETTING GEOMETRY
    ###################
    pack $base.fra73 \
        -in $base -anchor center -expand 0 -fill both -ipady 4 -padx 6 \
        -pady 5 -side top 
    pack $base.fra73.lab165 \
        -in $base.fra73 -anchor center -expand 0 -fill x -padx 5 -pady 5 \
        -side top 
    pack $base.fra73.fra23 \
        -in $base.fra73 -anchor center -expand 0 -fill both -padx 2 \
        -side left 
    grid rowconf $base.fra73.fra23 1 -weight 1
    grid rowconf $base.fra73.fra23 2 -weight 20
    grid $base.fra73.fra23.lab24 \
        -in $base.fra73.fra23 -column 0 -row 0 -columnspan 21 -rowspan 1 \
        -pady 2 -sticky ew 
    grid $base.fra73.fra23.lab40 \
        -in $base.fra73.fra23 -column 21 -row 0 -columnspan 20 -rowspan 1 \
        -pady 2 -sticky ew 
    grid $base.fra73.fra23.lab41 \
        -in $base.fra73.fra23 -column 41 -row 0 -columnspan 20 -rowspan 1 \
        -pady 2 -sticky ew 
    grid $base.fra73.fra23.lab42 \
        -in $base.fra73.fra23 -column 61 -row 0 -columnspan 21 -rowspan 1 \
        -pady 2 -sticky ew 
    grid $base.fra73.fra23.fra59 \
        -in $base.fra73.fra23 -column 81 -row 2 -columnspan 1 -rowspan 1 
    grid $base.fra73.fra23.fra54 \
        -in $base.fra73.fra23 -column 0 -row 2 -columnspan 82 -rowspan 1 \
        -sticky nesw 
    grid $base.fra73.fra23.lis57 \
        -in $base.fra73.fra23 -column 1 -row 2 -columnspan 20 -rowspan 1 \
        -ipadx 1 -pady 4 -sticky nesw 
    grid $base.fra73.fra23.lis58 \
        -in $base.fra73.fra23 -column 21 -row 2 -columnspan 20 -rowspan 1 \
        -pady 4 -sticky nesw 
    grid $base.fra73.fra23.lis59 \
        -in $base.fra73.fra23 -column 41 -row 2 -columnspan 20 -rowspan 1 \
        -pady 4 -sticky nesw 
    grid $base.fra73.fra23.lis60 \
        -in $base.fra73.fra23 -column 61 -row 2 -columnspan 20 -rowspan 1 \
        -pady 4 -sticky nesw 
    grid $base.fra73.fra23.scr25 \
        -in $base.fra73.fra23 -column 82 -row 2 -columnspan 1 -rowspan 1 \
        -sticky ns 
    pack $base.fra73.fra64 \
        -in $base.fra73 -anchor center -expand 1 -fill both -padx 12 -pady 6 \
        -side left 
    pack $base.fra73.fra64.but104 \
        -in $base.fra73.fra64 -anchor s -expand 1 -fill none -pady 5 \
        -side top 
    pack $base.fra73.fra64.but105 \
        -in $base.fra73.fra64 -anchor center -expand 0 -fill none -pady 3 \
        -side top 
    pack $base.fra73.fra64.but106 \
        -in $base.fra73.fra64 -anchor center -expand 0 -fill none -pady 5 \
        -side top 
    pack $base.fra18 \
        -in $base -anchor center -expand 1 -fill both -padx 2 -side left 
    pack $base.fra18.fra67 \
        -in $base.fra18 -anchor center -expand 0 -fill both -ipady 2 -padx 2 \
        -pady 2 -side top 
    pack $base.fra18.fra67.lab164 \
        -in $base.fra18.fra67 -anchor center -expand 0 -fill x -padx 5 \
        -pady 3 -side top 
    pack $base.fra18.fra67.rad72 \
        -in $base.fra18.fra67 -anchor center -expand 0 -fill x -padx 16 \
        -side top 
    pack $base.fra18.fra67.rad73 \
        -in $base.fra18.fra67 -anchor center -expand 0 -fill x -padx 16 \
        -side top 
    pack $base.fra18.fra67.rad74 \
        -in $base.fra18.fra67 -anchor center -expand 0 -fill x -padx 16 \
        -side top 
    pack $base.fra18.lab69 \
        -in $base.fra18 -anchor center -expand 1 -fill none -padx 2 -pady 6 \
        -side top 
    pack $base.fra19 \
        -in $base -anchor center -expand 1 -fill both -side left 
    pack $base.fra19.fra18 \
        -in $base.fra19 -anchor n -expand 1 -fill both -ipady 2 -padx 2 \
        -pady 2 -side top 
    pack $base.fra19.fra18.lab164 \
        -in $base.fra19.fra18 -anchor center -expand 0 -fill x -padx 5 \
        -pady 3 -side top 
    pack $base.fra19.fra18.fra19 \
        -in $base.fra19.fra18 -anchor center -expand 0 -fill x -side top 
    grid columnconf $base.fra19.fra18.fra19 0 -weight 1
    grid $base.fra19.fra18.fra19.rad20 \
        -in $base.fra19.fra18.fra19 -column 0 -row 0 -columnspan 1 -rowspan 1 \
        -padx 5 -sticky w 
    grid $base.fra19.fra18.fra19.rad21 \
        -in $base.fra19.fra18.fra19 -column 0 -row 1 -columnspan 1 -rowspan 1 \
        -padx 5 -sticky w 
    grid $base.fra19.fra18.fra19.che22 \
        -in $base.fra19.fra18.fra19 -column 1 -row 0 -columnspan 1 -rowspan 2 \
        -ipadx 11 -ipady 8 -padx 6 -pady 2 -sticky ns 
    pack $base.fra19.fra18.fra30 \
        -in $base.fra19.fra18 -anchor center -expand 1 -fill both -padx 7 \
        -pady 6 -side top 
    grid columnconf $base.fra19.fra18.fra30 0 -weight 1
    grid columnconf $base.fra19.fra18.fra30 1 -weight 10
    grid $base.fra19.fra18.fra30.lab26 \
        -in $base.fra19.fra18.fra30 -column 0 -row 1 -columnspan 1 -rowspan 1 \
        -ipadx 2 -padx 3 -pady 6 -sticky new 
    grid $base.fra19.fra18.fra30.lab27 \
        -in $base.fra19.fra18.fra30 -column 0 -row 2 -columnspan 1 -rowspan 1 \
        -ipadx 2 -padx 3 -pady 6 -sticky esw 
    grid $base.fra19.fra18.fra30.fra28 \
        -in $base.fra19.fra18.fra30 -column 1 -row 1 -columnspan 1 -rowspan 1 \
        -padx 3 -sticky new 
    pack $base.fra19.fra18.fra30.fra28.but78 \
        -in $base.fra19.fra18.fra30.fra28 -anchor w -expand 0 -fill none \
        -side left 
    pack $base.fra19.fra18.fra30.fra28.sca80 \
        -in $base.fra19.fra18.fra30.fra28 -anchor center -expand 1 -fill x \
        -padx 4 -side left 
    pack $base.fra19.fra18.fra30.fra28.but81 \
        -in $base.fra19.fra18.fra30.fra28 -anchor center -expand 0 -fill x \
        -side left 
    grid $base.fra19.fra18.fra30.fra29 \
        -in $base.fra19.fra18.fra30 -column 1 -row 2 -columnspan 2 -rowspan 1 \
        -padx 3 -sticky esw 
    pack $base.fra19.fra18.fra30.fra29.but78 \
        -in $base.fra19.fra18.fra30.fra29 -anchor center -expand 0 -fill none \
        -side left 
    pack $base.fra19.fra18.fra30.fra29.sca80 \
        -in $base.fra19.fra18.fra30.fra29 -anchor center -expand 1 -fill x \
        -padx 4 -side left 
    pack $base.fra19.fra18.fra30.fra29.but81 \
        -in $base.fra19.fra18.fra30.fra29 -anchor center -expand 0 -fill x \
        -side left 
    pack $base.fra19.fra19 \
        -in $base.fra19 -anchor center -expand 1 -fill x -side bottom 
    pack $base.fra19.fra19.but20 \
        -in $base.fra19.fra19 -anchor center -expand 1 -fill x -ipady 8 \
        -padx 2 -side left 
    pack $base.fra19.fra19.but21 \
        -in $base.fra19.fra19 -anchor center -expand 1 -fill x -ipady 8 \
        -padx 2 -side left 
    pack $base.fra19.fra19.but24 \
        -in $base.fra19.fra19 -anchor center -expand 1 -fill x -ipady 8 \
        -padx 2 -side left 
    pack $base.fra19.fra19.but25 \
        -in $base.fra19.fra19 -anchor center -expand 1 -fill x -ipady 8 \
        -padx 2 -side left 
    pack $base.fra19.fra19.but26 \
        -in $base.fra19.fra19 -anchor center -expand 1 -fill x -ipady 8 \
        -padx 2 -side left 

    vTcl:FireEvent $base <<Ready>>
}

proc vTclWindow.top74 {base {container 0}} {
    if {$base == ""} {
        set base .top74
    }
    if {[winfo exists $base] && (!$container)} {
        wm deiconify $base; return
    }

    global widget
    vTcl:DefineAlias "$base" "tlAddPanel" vTcl:Toplevel:WidgetProc "" 1
    vTcl:DefineAlias "$base.fra19" "Frame53" vTcl:WidgetProc "tlAddPanel" 1
    vTcl:DefineAlias "$base.fra19.but24" "btAddPanelOK" vTcl:WidgetProc "tlAddPanel" 1
    vTcl:DefineAlias "$base.fra19.but25" "btAddPanelCancel" vTcl:WidgetProc "tlAddPanel" 1
    vTcl:DefineAlias "$base.fra52" "Frame56" vTcl:WidgetProc "tlAddPanel" 1
    vTcl:DefineAlias "$base.fra52.fra51" "Frame55" vTcl:WidgetProc "tlAddPanel" 1
    vTcl:DefineAlias "$base.fra52.fra51.ent49" "etAddPanel" vTcl:WidgetProc "tlAddPanel" 1
    vTcl:DefineAlias "$base.fra52.fra51.lab48" "lbAddPanelInterface" vTcl:WidgetProc "tlAddPanel" 1

    ###################
    # CREATING WIDGETS
    ###################
    if {!$container} {
    vTcl:toplevel $base -class Toplevel
    wm withdraw $base
    wm focusmodel $base passive
    wm geometry $base 263x113+402+128; update
    wm maxsize $base 1009 738
    wm minsize $base 1 1
    wm overrideredirect $base 0
    wm resizable $base 0 0
    wm title $base "Add Panel"
    vTcl:FireEvent $base <<Create>>
    }
    frame $base.fra52 \
        -borderwidth 2 -height 75 -relief groove -width 125 
    frame $base.fra52.fra51 \
        -borderwidth 2 -height 75 -width 125 
    label $base.fra52.fra51.lab48 \
        -pady 1 -text Interface: 
    entry $base.fra52.fra51.ent49 \
        -background white -textvariable V_etAddPanel -width 22 
    bind $base.fra52.fra51.ent49 <Key-Escape> {
        $widget(btAddPanelCancel) invoke
    }
    bind $base.fra52.fra51.ent49 <Key-Return> {
        $widget(btAddPanelOK) invoke
    }
    bind $base.fra52.fra51.ent49 <KeyRelease> {
        set V_etAddPanel [string range $V_etAddPanel 0 19]
if { $_APOKP_err } {
    incr _APOKP_err -1
}
    }
    frame $base.fra19 \
        -borderwidth 2 -height 91 -width 161 
    button $base.fra19.but24 \
        -command _AddPanelOKPressed -height 1 -pady 1m -text OK -width 6 
    button $base.fra19.but25 \
        -command PopMainWindow -height 1 -text Cancel -width 6 
    ###################
    # SETTING GEOMETRY
    ###################
    pack $base.fra52 \
        -in $base -anchor center -expand 1 -fill both -side top 
    pack $base.fra52.fra51 \
        -in $base.fra52 -anchor center -expand 1 -fill none -ipadx 1 -ipady 1 \
        -side top 
    pack $base.fra52.fra51.lab48 \
        -in $base.fra52.fra51 -anchor center -expand 0 -fill none -padx 1 \
        -side left 
    pack $base.fra52.fra51.ent49 \
        -in $base.fra52.fra51 -anchor center -expand 0 -fill none -ipady 2 \
        -side left 
    pack $base.fra19 \
        -in $base -anchor center -expand 0 -fill both -padx 4 -pady 6 \
        -side top 
    pack $base.fra19.but24 \
        -in $base.fra19 -anchor e -expand 1 -fill none -ipady 5 -padx 4 \
        -side left 
    pack $base.fra19.but25 \
        -in $base.fra19 -anchor center -expand 0 -fill none -ipady 5 -padx 4 \
        -side left 

    vTcl:FireEvent $base <<Ready>>
}

bind "lbPanelList" <<ListboxSelect>> {
    #_ListboxSelect %W
PanelList select %W
}
bind "btAdvanced" <Button-1> {
    _AS_Button1 %W
}
bind "btAdvanced" <FocusIn> {
    _AS_FocusChange %W 1
}
bind "btAdvanced" <FocusOut> {
    _AS_FocusChange %W -1
}
bind "btAdvanced" <Key-KP_Left> {
    _AS_KeyLorR %W -1
}
bind "btAdvanced" <Key-KP_Right> {
    _AS_KeyLorR %W 1
}
bind "btAdvanced" <Key-Left> {
    _AS_KeyLorR %W -1
}
bind "btAdvanced" <Key-Right> {
    _AS_KeyLorR %W 1
}

Window show .
Window show .top18
Window show .top19
Window show .top41
Window show .top52
Window show .top63
Window show .top74

main $argc $argv
