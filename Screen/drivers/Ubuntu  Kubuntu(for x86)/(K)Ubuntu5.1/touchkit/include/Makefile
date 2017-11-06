
TCL		= tclsh
TCL2H	= ../utility/tcl2h.tcl
TCL2SH	= ../utility/tcl2sh.tcl
TCL2MAK	= ../utility/tcl2mak.tcl
INI2TCL	= ../utility/ini2tcl.tcl

TARGET	= configSTR.h configSTR.mak \
		configINT.h configINT.mak	\
		touch.tcl
			
all auto rebuild:	$(TARGET)

new:	clean all

clean:
	rm -f $(TARGET)

%.h: %.tcl $(TCL2H)
	$(TCL) $(TCL2H) $< > $@

%.sh: %.tcl $(TCL2SH)
	$(TCL) $(TCL2SH) $< > $@

%.mak: %.tcl $(TCL2MAK)
	$(TCL) $(TCL2MAK) $< > $@

%.tcl: %.ini $(INI2TCL)
	$(TCL) $(INI2TCL) $< > $@

