
SUBDIRS = include driver utility xf86drv diag usb

MAKE_OPT = -s

##################################################################

TCL = $(shell which tcl 2>/dev/null)
TCLSH = $(shell which tclsh 2>/dev/null)

ifeq ($(TCL),)
ifeq ($(TCLSH),)
	EXEC := $(shell echo '(E) Tcl/Tk not installed' > /dev/tty)
	error stop
else
	TCL := $(shell echo $(TCLSH) | sed 's/tclsh/tcl/')
	EXEC := $(shell ln -s $(TCLSH) $(TCL))
endif
endif

MODULE_HOME = $(shell find /lib/modules -name usb -type d)

ifeq ($(MODULE_HOME),)
	MODULE_HOME = ""
endif

XMODULE_HOME = /usr/X11R6/lib/modules/

#XINITRC_D = $(shell find /etc -type d -name xinit\*.d)
XINITRC_D = /etc/X11/Xsession.d/
STARTUP_NAME = 50xfree86-common_determine-startup

GLOBAL_SETTING := configSTR.mak
CMD := $(shell make -C include $(GLOBAL_SETTING))
ifneq ($(CMD),0)
	include include/$(GLOBAL_SETTING)
endif

GLOBAL_SETTING := configINT.mak
CMD := $(shell make -C include $(GLOBAL_SETTING))
ifneq ($(CMD),0)
	include include/$(GLOBAL_SETTING)
endif

PRJ_HOME = $($(PRJNAME)_HOME)
PRJ_UTILITY_HOME = $(PRJ_HOME)utility/
PRJ_INCLUDE_HOME = $(PRJ_HOME)include/
PRJ_IMAGE_HOME = $(PRJ_HOME)image/
PRJ_DIAG_HOME = $(PRJ_HOME)diag/

XMODULE = xf86$(MODULE_NAME).so
XDRIVER = $(XDRIVER_NAME)_drv.o

all: 
	for n in $(SUBDIRS); do				\
		$(MAKE) -C $$n || exit 1;		\
	done
	: '+-----------------------------------+'
	: '|  Build-All Complete Successfully  |'
	: '+-----------------------------------+'

clean:
	rm -f *.bak core DEADJOE
	Reverse="" ;					\
	for n in $(SUBDIRS); do			\
		Reverse="$$n $$Reverse" ; 	\
	done;							\
	echo $$Reverse;					\
	for n in $$Reverse; do			\
		$(MAKE) -C $$n clean;		\
	done

new: clean all

##################################################################

USB_PRESENT = $(shell test -d /proc/bus/usb && \
				 ! test -f /tmp/.usb.on -a `cat /tmp/.usb.on` = 0; expr 1 - $$?)

$(GLOGAL_SETTING):
	$(MAKE) -C $$n $@ || exit 1;

ifeq ($(shell whoami),root)

install:
	@make $(MAKE_OPT) _install

uninstall: $(PRJ_HOME)Makefile
	@make $(MAKE_OPT) _uninstall

else

install:
	:
	: You must have root privilege to do this
	:
		
uninstall:
	:
	: You must have root privilege to do this
	:

endif

DIRECTORY = $(PRJ_HOME) $(PRJ_INCLUDE_HOME) $(PRJ_UTILITY_HOME) \
			$(PRJ_DIAG_HOME) $(PRJ_IMAGE_HOME)

UNINSTALL = uninstall_$(PRJNAME)
	
_install:
	for n in $(DIRECTORY) ; do					\
		: echo "(*) Create directory [$$n]";	\
		if [ ! -d $$n ] ; then					\
			mkdir $$n || exit 1;				\
		fi;										\
	done
	:
	cp -f include/configSTR.tcl include/configINT.tcl \
			include/configSTR.mak include/configINT.mak \
			include/touch.tcl \
			$(PRJ_INCLUDE_HOME)
	:
	if [ $(USB_PRESENT) -eq 1 ] ; then             					\
		cp -f usb/usbpnpd $(PRJ_HOME)$(USBPNPD_NAME);				\
		rm -f /usr/bin/$(USBPNPD_NAME);								\
		ln -s $(PRJ_HOME)$(USBPNPD_NAME) /usr/bin;					\
		tcl utility/usbdcfg.tcl;									\
		:;															\
		rm -f $(USBPORT)*;											\
		i=0; while [ $$i -lt $(SCR_MAX_MNR) ] ; do					\
			mknod $(USBPORT)$$i c 180 `expr $(SCR_BASE_MNR) + $$i`;	\
			i=`expr $$i + 1`;										\
		done;														\
		if [ -f usb/tkusb.ko ] ; then								\
			for d in $(MODULE_HOME) ; do							\
				printf "(*) Install USB module";					\
				echo " [$$d/$(USBMODULE_NAME).ko]";					\
				cp -f usb/tkusb.ko $$d/$(USBMODULE_NAME).ko;			\
			done;													\
		fi;															\
	fi
	:
	echo "(*) Install touch panel daemon [/usr/bin/$(TPANELD_NAME)]"
	if [ ! -d $($(PRJNAME)_ROMHOME) ] ; then			\
		mkdir $($(PRJNAME)_ROMHOME);					\
	fi
	cp -f driver/tpaneld $(PRJ_HOME)$(TPANELD_NAME)
	chmod 4755 $(PRJ_HOME)$(TPANELD_NAME)
	rm -f /usr/bin/$(TPANELD_NAME)
	ln -s $(PRJ_HOME)$(TPANELD_NAME) /usr/bin/
	if [ "$(XINITRC_D)" != "" ] ; then										\
		cp $(XINITRC_D)/$(STARTUP_NAME) $(XINITRC_D)/$(STARTUP_NAME).bak;	\
		echo "#!/bin/sh" >> $(XINITRC_D)/$(STARTUP_NAME);					\
		echo "/usr/bin/$(TPANELD_NAME) -f &" >> $(XINITRC_D)/$(STARTUP_NAME);\
	else																	\
		pushd . > /dev/null;												\
		cd;																	\
		home=`pwd`;															\
		popd > /dev/null;													\
		if [ ! -f $$home/.Xclients.bak.tp ] ; then							\
			cp -f $$home/.Xclients $$home/.Xclients.bak.tp;					\
		fi;																	\
		echo "/usr/bin/$(TPANELD_NAME) &" > $$home/.Xclients;				\
		if [ -f $$home/.Xclients.bak.tp ] ; then							\
			cat $$home/.Xclients.bak.tp >> $$home/.Xclients;				\
		fi;																	\
	fi
	:
	echo "(*) Install configuration utility [/usr/bin/$(TOUCHCFG_NAME)]"
	cp -f utility/PanelInfo utility/library.tcl			\
			utility/rclocal.tcl utility/xf86cfg.tcl		\
			utility/xversion.tcl utility/shortcut.tcl	\
			utility/WriteConf $(PRJ_UTILITY_HOME) 
	chmod 4755 $(PRJ_UTILITY_HOME)PanelInfo $(PRJ_UTILITY_HOME)WriteConf
	cp -f image/* $(PRJ_IMAGE_HOME)
	cp -f diag/*.t?? $(PRJ_DIAG_HOME)
	cp -f diag/drawtest $(PRJ_DIAG_HOME)$(DRAW_NAME)
	cp -f diag/25pcal $(PRJ_DIAG_HOME)$(_25PCAL_NAME)
	cp -f diag/4pcal $(PRJ_DIAG_HOME)$(_4PCAL_NAME)
	chmod 4755 $(PRJ_DIAG_HOME)$(DRAW_NAME) $(PRJ_DIAG_HOME)$(_25PCAL_NAME) \
					$(PRJ_DIAG_HOME)$(_4PCAL_NAME)
	:
	: Generate $(TOUCHCFG_NAME)
	echo "#!/bin/sh" > $(PRJ_HOME)$(TOUCHCFG_NAME)
	echo "echo puts [catch {wm withdraw .}]\; exit > /tmp/exit_test" >> $(PRJ_HOME)$(TOUCHCFG_NAME)
	echo "rval=\`wish /tmp/exit_test \$$* 2>&1\`" >> $(PRJ_HOME)$(TOUCHCFG_NAME)
	echo "rm -f /tmp/exit_test" >> $(PRJ_HOME)$(TOUCHCFG_NAME)
	: echo "echo \$$rval" >> $(PRJ_HOME)$(TOUCHCFG_NAME)
	echo "if [ \"\$$rval\" == 0 ] ; then" >> $(PRJ_HOME)$(TOUCHCFG_NAME)
	echo "	exec $(PRJ_DIAG_HOME)touchcfg.tcl $(PRJ_HOME) \$$*" >> $(PRJ_HOME)$(TOUCHCFG_NAME)
	echo "else" >> $(PRJ_HOME)$(TOUCHCFG_NAME)
	echo "	echo can not open display" >> $(PRJ_HOME)$(TOUCHCFG_NAME)
	echo "fi" >> $(PRJ_HOME)$(TOUCHCFG_NAME)
	chmod 755 $(PRJ_HOME)$(TOUCHCFG_NAME)
	: Generation OK
	:
	rm -f /usr/bin/$(TOUCHCFG_NAME)
	ln -s $(PRJ_HOME)$(TOUCHCFG_NAME) /usr/bin
	:
	if [ 430 -lt 400 ] ; then						\
		echo "(*) Install XFree86 driver [$(XMODULE_HOME)$(XMODULE)]";	\
		cp -f xf86drv/bin/xf86TouchKit.so $(XMODULE_HOME)$(XMODULE);	\
		chmod 755 $(XMODULE_HOME)$(XMODULE);							\
	else																\
		echo "(*) Install XFree86 driver [$(XMODULE_HOME)input/$(XDRIVER)]";\
		cp -f xf86drv/bin/touchkit_drv.o $(XMODULE_HOME)input/$(XDRIVER);\
		chmod 755 $(XMODULE_HOME)input/$(XDRIVER);						\
	fi
	:
	# Need shortcut for TOUCHCFG?
	:
	echo "(*) Generate uninstall script [/usr/bin/$(UNINSTALL)]"
	echo '#!/bin/sh'							> $(PRJ_HOME)$(UNINSTALL)
	echo 'echo ===== $(PRJNAME) Uninstaller =====' >> $(PRJ_HOME)$(UNINSTALL)
	echo 'printf "Are you sure to remove driver and utility? (Y)es/(N)o [N]"' >> $(PRJ_HOME)$(UNINSTALL)
	echo 'read cmd'								>> $(PRJ_HOME)$(UNINSTALL)
	echo 'case $$cmd in'						>> $(PRJ_HOME)$(UNINSTALL)
	echo '	[yY]|[yY][eE][sS]) :;;'				>> $(PRJ_HOME)$(UNINSTALL)
	echo '	*) exit 0;;'						>> $(PRJ_HOME)$(UNINSTALL)
	echo 'esac'									>> $(PRJ_HOME)$(UNINSTALL)
	echo 'cd /'									>> $(PRJ_HOME)$(UNINSTALL)
	echo 'make -C $(PRJ_HOME) -s uninstall'	>> $(PRJ_HOME)$(UNINSTALL)
	: Generation OK
	chmod 754 $(PRJ_HOME)$(UNINSTALL)
	rm -rf /usr/bin/$(UNINSTALL)
	ln -s $(PRJ_HOME)$(UNINSTALL) /usr/bin/$(UNINSTALL)
	cp -f Makefile $(PRJ_HOME)
	:
	: update starting up scripts
	tcl utility/rclocal.tcl
	tcl utility/xf86cfg.tcl
	:
	: create desktop shortcuts for KDE and GNome
	tcl utility/shortcut.tcl $(PRJ_IMAGE_HOME)
	:
	: copy tpaneld.ini to /etc
	cp -f driver/tpaneld.ini /etc
	:
	echo "+--------------------------------------+"
	echo "|  Installation Complete Successfully  |"
	echo "+--------------------------------------+"

		
_uninstall:
	tcl utility/rclocal.tcl remove
	tcl utility/xf86cfg.tcl remove
	:
	echo "(*) Uninstall touch panel daemon [/usr/bin/$(TPANELD_NAME)]"
	killall -KILL $(TPANELD_NAME) 2> /dev/null || true
	rm -f /usr/bin/$(TPANELD_NAME)
	rm -f  $($(PRJNAME)_CMDIN) $($(PRJNAME)_CMDOUT) 	\
			$($(PRJNAME)_SHRIN) $($(PRJNAME)_SHROUT)	\
			$($(PRJNAME)_CONF) $($(PRJNAME)_LOCK)		\
			$(TOUCHCFG_CMD) $($(PRJNAME)_DAT)[0-9]*
	rm -rf $($(PRJNAME)_ROMHOME)
	if [ "$(XINITRC_D)" != "" ] ; then									\
		cp $(XINITRC_D)/$(STARTUP_NAME).bak $(XINITRC_D)/$(STARTUP_NAME);	\
		rm -f $(XINITRC_D)/$(STARTUP_NAME).bak;							\
	else																\
		pushd . > /dev/null;											\
		cd;																\
		home=`pwd`;														\
		popd > /dev/null;												\
		if [ -f $$home/.Xclients.bak.tp ] ; then						\
			mv -f $$home/.Xclients.bak.tp $$home/.Xclients;				\
		fi;																\
	fi
	:
	rcpid=`ps x | grep wish`;							\
	rcpid=`echo $$rcpid | grep rbutton.tcl`;			\
	if [ $$? -eq 0 ] ; then								\
		rcpid=`echo $$rcpid | awk '{ print $$1; }'`;	\
		killall -KILL $$rcpid 1> /dev/null 2>&1 || true;	\
	fi
	:
	echo "(*) Uninstall configuration utility [/usr/bin/$(TOUCHCFG_NAME)]"
	rm -f /usr/bin/$(TOUCHCFG_NAME)
	:
	if [ 430 -lt 400 ] ; then						\
		echo "(*) Uninstall XFree86 driver [$(XMODULE_HOME)$(XMODULE)]";\
		rm -f $(XMODULE_HOME)$(XMODULE);								\
	else																\
		echo "(*) Uninstall XFree86 driver [$(XMODULE_HOME)input/$(XDRIVER)]";\
		rm -f $(XMODULE_HOME)input/$(XDRIVER);							\
	fi
	:
	if [ $(USB_PRESENT) -eq 1 ] ; then             					\
		killall -KILL $(USBPNPD_NAME) > /dev/null 2>&1 || true;		\
		rm -f /usr/bin/$(USBPNPD_NAME);								\
		rm -f $(PNP_CONFIG);										\
		:;															\
		rm -f $(USBPORT)*;											\
		if lsmod | grep $(USBMODULE_NAME) > /dev/null 2>&1 ; then	\
			rmmod $(USBMODULE_NAME) || true;						\
		fi;															\
		for d in $(MODULE_HOME) ; do								\
			if [ -f $$d/$(USBMODULE_NAME).ko ] ; then				\
				printf "(*) Remove USB module";						\
				echo " [$$d/$(USBMODULE_NAME).ko]";					\
				rm -f $$d/$(USBMODULE_NAME).ko;						\
			fi;														\
		done;														\
	fi
	:
	tcl utility/shortcut.tcl remove
	:
	:
	rm -f /etc/tpaneld.ini
	:
	echo "(*) Remove directory recursively [$(PRJ_HOME)]"
	cd /;												\
	rm -rf $(PRJ_HOME);									\
	rm -f /usr/bin/$(UNINSTALL);						\
	echo "+----------------------------------------+";	\
	echo "|  Uninstallation Complete Successfully  |";	\
	echo "+----------------------------------------+"

activate:
	@killall -KILL $(TPANELD_NAME) 2> /dev/null || true
	@/usr/bin/$(TPANELD_NAME)
	@sleep 2
	@echo '(I) Start $(PRJNAME) touch panel daemon';		
	@if [ $(USB_PRESENT) -eq 1 ] ; then             			\
		killall -KILL $(USBPNPD_NAME) 2> /dev/null || true;		\
		rmmod $(USBMODULE_NAME) > /dev/null 2>&1 || true;		\
		/usr/bin/$(USBPNPD_NAME);								\
		if lsmod | grep $(USBMODULE_NAME) > /dev/null 2>&1 ; then\
			echo '(I) Start USB module';						\
		fi;														\
	fi

auto:
	for n in $(SUBDIRS); do				\
		$(MAKE) -C $$n $@ || exit 1;	\
	done

rebuild:
	for n in $(SUBDIRS); do				\
		$(MAKE) -C $$n $@ || exit 1;	\
	done
	: '+-----------------------------------+'
	: '|  Build-All Complete Successfully  |'
	: '+-----------------------------------+'
	

