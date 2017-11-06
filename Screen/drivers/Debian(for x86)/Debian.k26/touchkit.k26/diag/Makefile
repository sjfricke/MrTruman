
TARGET = 4pcal 25pcal drawtest

###############################################################

COPY_LIST = $(TARGET) rbutton.tcl

all: rebuild
	for t in $(COPY_LIST); do		\
		if [ -f $$t ]; then			\
			cp -f $$t bin;			\
		fi;							\
	done

rebuild: Makefile rbutton.tcl
	make -f Makefile
	for t in $(TARGET); do			\
		if [ -f $$t ]; then			\
			strip $$t;				\
		fi;							\
	done

auto:
	done=false;							\
	for t in $(COPY_LIST); do			\
		if [ -f bin/$$t ];  then		\
			cp -f bin/$$t .;			\
			done=true;					\
		fi;								\
	done;								\
	$$done

new: clean all

clean: 
	rm -f $(COPY_LIST) *.o core Makefile Imakefile *.bak *.tmp

Makefile: Imakefile
	xmkmf

Imakefile: makefile
	echo 'CFLAGS = -Wall -ggdb2 -I ../include' > $@
	echo 'COMMON_HEADER = ../include/eeprom.c' >> $@
	echo 'LOCAL_LIBRARIES = $$(XLIB)' >> $@
	for t in $(TARGET); do 			\
		echo "all:: $$t" >> $@;		\
		echo 'NormalProgramTarget('$$t,$$t'.o,$$(COMMON_HEADER),$$(LOCAL_LIBRARIES),)' >> $@;	\
	done

rbutton.tcl: rbutton.base makefile
	echo '#!/bin/sh' > $@
	echo '# \' >> $@
	echo 'exec wish "$$0" "$$@"' >> $@
	cat ../include/configINT.tcl >> $@
	cat ../include/configSTR.tcl >> $@
	cat ../include/touch.tcl >> $@
	cat $< >> $@
	chmod 755 $@
	
	
