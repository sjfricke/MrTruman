DAEMON = usbpnpd

GCC = gcc
INCLUDE = -I ../include
C_FLAGS = -Wall -O -pipe $(INCLUDE)
GLOBAL = ../include/configINT.tcl ../include/configSTR.tcl

ifneq ($(KERNELRELEASE),)
obj-m := tkusb.o
else
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)


auto:$(DAEMON) build_module
	 

all: $(DAEMON) build_module
$(DAEMON): $(DAEMON).c Makefile $(GLOBAL)
	echo $(GCC) -o $@ $(DAEMON).c $(C_FLAGS) $(L_FLAGS)
	$(GCC) -o $@ $(DAEMON).c $(C_FLAGS) $(L_FLAGS)
	strip $@
	
build_module:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	
clean:
	rm -f tkusb.o tkusb.mod.o tkusb.ko $(DAEMON) tkusb.mod.c
endif
